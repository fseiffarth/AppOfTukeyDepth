#include <iostream>
#include <cstring>
#include <chrono>
#include <filesystem>
#include <vector>
#include <DataClasses.h>
#include <LoadSave.h>
#include <GraphFunctions.h>
#include <FileEvaluation.h>
#include <io.h>
#include <omp.h>
#include "Algorithm/TukeyDepth.h"

int main(int argc, char *argv[]) {
    // -t 24 -db BZR AIDS DHFR Mutagenicity MUTAG NCI1 NCI109 PTC_FM PTC_FR PTC_MM PTC_MR COIL-DEL Cuneiform MSRC_9 MSRC_21 MSRC_21C DD ENZYMES OHSU KKI Peking_1 PROTEINS aspirin benzene ethanol
    std::string argument;
    std::string input_path = "../../Graphs/";
    std::string output_path = "../../out/";
    Indices num_threads = 1;
    int geodesic_distance = -1;
    for (Indices i = 0; i < argc; ++i) {
        std::string str_argument = std::string(argv[i]);
        bool str_argument_key = str_argument[0] == '-';

        if (str_argument_key) {
            argument.clear();
            argument = argv[i];
        }
            // List arguments
        else {
            if (argument == "-t") {
                num_threads = std::stoi(argv[i]);
            } else if (argument == "-path") {
                input_path = std::string(argv[i]);
            } else if (argument == "-out") {
                output_path = std::string(argv[i]);
            }else if (argument == "-distance") {
                geodesic_distance = std::stoi(argv[i]);
            }
        }
    }

    if(!input_path.empty() && (std::filesystem::path(input_path).extension() == ".edges" || std::filesystem::path(input_path).extension() == ".bin")){
        GraphData graphs;
        graphs.add(GraphStruct(input_path,false));
        std::string db = std::filesystem::path(input_path).stem();
        Indices deleted = 0;
        for (Indices i = 0; i < graphs.size(); ++i) {
            if (!GraphFunctions::IsConnected(graphs[i])) {
                graphs.graphData.erase(graphs.graphData.begin() + i);
                --i;
                ++deleted;
            }
        }
        std::cout << "Calculating the tukey depth for " << db << ". ";
        if(deleted > 0) {
            std::cout << deleted << " were not connected and hence deleted." << std::endl;
        }

        auto start = std::chrono::system_clock::now();
        std::vector<Indices> depths;
        TukeyDepth::run_parallel(0, graphs[0], depths, num_threads, geodesic_distance);

        std::string geo_str;
        if (geodesic_distance != -1) {
            geo_str = "_" + std::to_string(geodesic_distance);
        }
        auto time = (double) std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now() - start).count() / 1000;
        std::string name = db + ".tukey" + geo_str;

        FileEvaluation f = FileEvaluation(output_path, db, "tukey" + geo_str + "_info");
        f.headerValueInsert({"Name", "Graphs", "Connected", "Runtime"}, {db, std::to_string(graphs.graphData.size() + deleted), std::to_string(graphs.graphData.size()), std::to_string(time)});
        f.save();
        std::vector<std::vector<Indices>> graph_depths;
        graph_depths.emplace_back(depths);
        DataIO<Indices>::WriteTrivialMatrix(output_path + name, graph_depths);
    }
    else{
        std::cout << "Please give some database via -db <db_name> or provide a path to a graph in the edge_list format with -path <path_to_graph>" << std::endl;
    }
    return 0;
}
