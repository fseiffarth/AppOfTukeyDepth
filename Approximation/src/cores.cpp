//
// Created by florian on 17.05.22.
//

#include <string>
#include <io.h>
#include "../Data/GraphData.h"
#include "../Utils/LoadTUDortmundGraphData.h"
#include "../Algorithms/TukeyDepth.h"

typedef std::vector<std::vector<int>> DEPTHS;

struct Parameters{
    const std::string path = "../../Graphs/";
    const std::string db = "MUTAG";
    int threads = 1;
};

void core(std::string input_path,std::string output_path, int num_threads) {
    std::vector<GraphData> data;
    GraphData graph = GraphData(input_path);
    data.emplace_back(graph);
    std::string db = std::filesystem::path(input_path).stem();
    int deleted = 0;
    for (int i = 0; i < data.size(); ++i) {
        if (!TSnap::IsConnected(data[i].get_graph())) {
            data.erase(data.begin() + i);
            --i;
            ++deleted;
        }
    }
    std::cout << "Calculating the core for " << data.size() << "/" << (int) data.size() + deleted
              << " graphs in " << db << ". " << deleted << " were not connected and hence deleted." << std::endl;

    int max = 0;
    int min = std::numeric_limits<int>::max();
    for (auto const& x : data) {
        max = std::max(max, (int) x.size());
        min = std::min(min, (int) x.size());
    }

    std::cout << "Max graph size: " << max  << " Min graph size: " << min << std::endl;

    std::string name = db + ".core";


    //TODO use other node features for classification
    auto start = std::chrono::system_clock::now();
    std::vector<std::vector<int>> features_second;
    start = std::chrono::system_clock::now();
    std::vector<std::vector<std::string>> d = std::vector<std::vector<std::string>>(data.size(), std::vector<std::string>());
    TIntV coreNodes;
    GraphFunctions::get_core(graph, std::min(5, min), 3, 0, coreNodes);
    for (int j = 0; j < coreNodes.Len(); ++j) {
        d[0].emplace_back(std::to_string(coreNodes[j]));
    }
    StaticFunctionsLib::write_csv(output_path + name, d);
}


void cores(const std::string& input_path,const std::string& output_path,const std::string& db, int num_threads){
    std::vector<GraphData> data;
    std::vector<int> graphLabels;
    std::vector<std::vector<int>> graphEdgeLabels;
    LoadTUDortmundGraphData::GraphDataToGraphList(input_path, db, data, graphLabels, &graphEdgeLabels);
    int deleted = 0;
    for (int i = 0; i < data.size(); ++i) {
        if (!TSnap::IsConnected(data[i].get_graph())) {
            data.erase(data.begin() + i);
            --i;
            ++deleted;
        }
    }
    std::cout << "Calculating the core for " << data.size() << "/" << (int) data.size() + deleted
              << " graphs in " << db << ". " << deleted << " were not connected and hence deleted." << std::endl;

    std::string name = db + ".core";


    //TODO use other node features for classification
    auto start = std::chrono::system_clock::now();
    std::vector<std::vector<int>> features_second;
    start = std::chrono::system_clock::now();
    std::vector<std::vector<std::string>> d = std::vector<std::vector<std::string>>(data.size(), std::vector<std::string>());
#pragma opm parallel for shared(data, min) default(none)
    for(int i = 0; i < data.size(); ++i) {
        GraphData& graph = data[i];
        TIntV coreNodes;
        GraphFunctions::get_core(graph, std::min(10,(int) graph.size()), 3, 0, coreNodes);
#pragma omp critical
        {
            for (int j = 0; j < coreNodes.Len(); ++j) {
                d[i].emplace_back(std::to_string(coreNodes[j]));
            }
        }
    }
    StaticFunctionsLib::write_csv(output_path + name, d);
}


int main(int argc, char *argv[]) {
    // -t 24 -path ../../Graphs/KarateClub.edges
    std::vector<std::string> dbs;
    std::string argument;
    std::string input_path = "../../Graphs/";
    std::string output_path = "../../out/";

    int num_threads = 1;
    for (int i = 0; i < argc; ++i) {
        std::string str_argument = std::string(argv[i]);
        bool str_argument_key = str_argument[0] == '-';

        if (str_argument_key){
            argument.clear();
            if (std::strcmp(argv[i], "-t") == 0)
            {
                argument = "threads";
            }
            else if (std::strcmp(argv[i], "-db") == 0 ) {
                argument = "db";
            }
            else if (std::strcmp(argv[i], "-i") == 0 ) {
                argument = "input";
            }
            else if (std::strcmp(argv[i], "-o") == 0 ) {
                argument = "output";
            }
        }
            // List arguments
        else{
            if (argument == "db")
            {
                dbs.emplace_back(std::string(argv[i]));
            }
            else if (argument == "threads")
            {
                num_threads = std::stoi(argv[i]);
            }
            else if (argument == "input"){
                input_path = std::string(argv[i]);
            }
            else if (argument == "output"){
                output_path = std::string(argv[i]);
            }
        }
    }
    if (!dbs.empty()) {
        for (auto const& db : dbs) {
            std::cout << "Calculating the cores for all graphs in " << db << std::endl;
            cores(input_path, output_path, db, num_threads);
        }
    }
    else if (!input_path.empty()){
        std::cout << "Calculating the core for graph " << std::filesystem::path(input_path).stem() << std::endl;
        core(input_path, output_path, num_threads);
    }
    else{
        std::cout << "Please give some database via -db " << std::endl;
    }
}

