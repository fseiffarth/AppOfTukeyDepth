//
// Created by florian on 17.05.22.
//

#include <string>
#include <io.h>
#include "../Data/GraphData.h"
#include "../Utils/LoadTUDortmundGraphData.h"
#include "../Algorithms/TukeyDepth.h"

typedef std::vector<std::vector<int>> DEPTHS;

void approx_tukey(const std::string& input_path, const std::string& output_path, const std::string& db, int num_threads){
    std::string db_name = db;
    std::vector<GraphData> data;
    int deleted = 0;
    if (!db_name.empty()) {
        std::vector<int> graphLabels;
        std::vector<std::vector<int>> graphEdgeLabels;
        LoadTUDortmundGraphData::GraphDataToGraphList(input_path, db_name, data, graphLabels, &graphEdgeLabels);
    }
    else if (!input_path.empty()){
        db_name = std::filesystem::path(input_path).stem();
        data.emplace_back(GraphData(input_path));
    }
    for (int i = 0; i < data.size(); ++i) {
        if (!TSnap::IsConnected(data[i].get_graph())) {
            data.erase(data.begin() + i);
            --i;
            ++deleted;
        }
    }
    std::cout << "Calculating the tukey depth for " << data.size() << "/" << (int) data.size() + deleted
                  << " graphs in " << db_name << ". " << deleted << " were not connected and hence deleted." << std::endl;




        //data = {data[2765]};
        std::string name = db_name + ".approx_tukey";

        auto start = std::chrono::system_clock::now();
        std::vector<std::vector<int>> features_second;
        start = std::chrono::system_clock::now();
        GetWeakTukeyDepth(data, features_second, num_threads, 0, TDAlgorithm::TUKEY_APPROX_OPTIMIZED);
        auto runtime = (double) std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now() - start).count() / 1000;

        FileEvaluation f = FileEvaluation(output_path, db_name, +"approx_tukey_info");
        f.headerValueInsert({"Name", "Graphs", "Connected", "Runtime"}, {db_name, std::to_string(data.size() + deleted), std::to_string(data.size()), std::to_string(runtime)});
        f.save();


        DataIO<int>::WriteTrivialMatrix(output_path + name + "_optimized", features_second);

        std::cout << "____________________________________________________________________________________"
                  << std::endl;
        std::cout << "Evaluation" << std::endl << std::endl;
        std::cout << "Time: " << runtime << std::endl;
}

int main(int argc, char *argv[]) {
    // -t 24 -db BZR AIDS DHFR Mutagenicity MUTAG NCI1 NCI109 PTC_FM PTC_FR PTC_MM PTC_MR COIL-DEL Cuneiform MSRC_9 MSRC_21 MSRC_21C DD ENZYMES OHSU KKI Peking_1 PROTEINS aspirin benzene ethanol
    std::vector<std::string> dbs;
    std::string argument;
    std::string input_path = "../../Graphs/";
    std::string output_path = "../../out/";
    int num_threads = 1;
    for (int i = 0; i < argc; ++i) {
        std::string str_argument = std::string(argv[i]);
        bool str_argument_key = str_argument[0] == '-';

        if (str_argument_key) {
            argument.clear();
            if (std::strcmp(argv[i], "-t") == 0) {
                argument = "threads";
            } else if (std::strcmp(argv[i], "-db") == 0) {
                argument = "db";
            } else if (std::strcmp(argv[i], "-i") == 0) {
                argument = "input";
            } else if (std::strcmp(argv[i], "-o") == 0) {
                argument = "output";
            } else if (std::strcmp(argv[i], "-d") == 0) {
                argument = "distance";
            }
        }
            // List arguments
        else {
            if (argument == "db") {
                dbs.emplace_back(std::string(argv[i]));
            } else if (argument == "threads") {
                num_threads = std::stoi(argv[i]);
            } else if (argument == "input") {
                input_path = std::string(argv[i]);
            } else if (argument == "output") {
                output_path = std::string(argv[i]);
            }
        }
    }
    if (!dbs.empty()) {
        for (auto const& db : dbs) {
            std::cout << "Calculating the tukey depth for all graphs in " << db << std::endl;
            approx_tukey(input_path, output_path, db, num_threads);
        }
    }
    else if (!input_path.empty()){
        approx_tukey(input_path, output_path, "", num_threads);
    }
    else{
        std::cout << "Please give some database via -db " << std::endl;
    }
}
