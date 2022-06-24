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

void approx_tukey(const std::string& db, int num_threads){
        std::vector<GraphData> data;
        std::vector<int> graphLabels;
        std::vector<std::vector<int>> graphEdgeLabels;
        Parameters params = {"../../Graphs/", db, num_threads};
        LoadTUDortmundGraphData::GraphDataToGraphList(params.path, params.db, data, graphLabels, &graphEdgeLabels);
        int deleted = 0;
        for (int i = 0; i < data.size(); ++i) {
            if (!TSnap::IsConnected(data[i].get_graph())) {
                data.erase(data.begin() + i);
                --i;
                ++deleted;
            }
        }
        std::cout << "Calculating the tukey depth for " << data.size() << "/" << (int) data.size() + deleted
                  << " graphs in " << db << ". " << deleted << " were not connected and hence deleted." << std::endl;




        //data = {data[2765]};

        std::string path = "../out/";
        std::string name = params.db + ".approx_tukey";

        auto start = std::chrono::system_clock::now();
        std::vector<std::vector<int>> features_second;
        start = std::chrono::system_clock::now();
        GetWeakTukeyDepth(data, features_second, params.threads, 0, TDAlgorithm::TUKEY_APPROX_OPTIMIZED);
        auto second_time = (double) std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now() - start).count() / 1000;

        FileEvaluation f = FileEvaluation(path, params.db, +"approx_tukey_info");
        f.headerValueInsert({"Name", "Graphs", "Connected", "Runtime"}, {db, std::to_string(data.size() + deleted), std::to_string(data.size()), std::to_string(second_time)});
        f.save();


        DataIO<int>::WriteTrivialMatrix(path + name + "_optimized", features_second);

        std::cout << "____________________________________________________________________________________"
                  << std::endl;
        std::cout << "Evaluation" << std::endl << std::endl;
        std::cout << "Time: " << second_time << std::endl;
}

int main(int argc, char *argv[]) {
    // -t 24 -db BZR AIDS DHFR Mutagenicity MUTAG NCI1 NCI109 PTC_FM PTC_FR PTC_MM PTC_MR COIL-DEL Cuneiform MSRC_9 MSRC_21 MSRC_21C DD ENZYMES OHSU KKI Peking_1 PROTEINS aspirin benzene ethanol
    std::vector<std::string> dbs;
    std::string argument;
    int num_threads = 1;
    for (int i = 0; i < argc; ++i) {
        std::string str_argument = std::string(argv[i]);
        bool str_argument_key = str_argument[0] == '-';

        if (str_argument_key){
            argument.clear();
            if (std::strcmp(argv[i], "-t") == 0)
            {
                argument = "t";
            }
            else if (std::strcmp(argv[i], "-db") == 0 ) {
                argument = "db";
            }
        }
        // List arguments
        else{
            if (argument == "db")
            {
                dbs.emplace_back(std::string(argv[i]));
            }
            else if (argument == "t")
            {
                num_threads = std::stoi(argv[i]);
            }
        }
    }
    if (!dbs.empty()) {
        for (auto const& db : dbs) {
            std::cout << "Calculating the tukey depth for all graphs in " << db << std::endl;
            approx_tukey(db, num_threads);
        }
    }
    else{
        std::cout << "Please give some database via -db " << std::endl;
    }
}
