//
// Created by florian on 17.05.22.
//

#include <string>
#include <io.h>
#include <FileEvaluation.h>
#include <limits>
#include <vector>
#include <cstring>

typedef std::vector<std::vector<int>> DEPTHS;

void compare_tukey(const std::string& input_path, const std::string& output_path, const std::string& db, FileEvaluation& fileEvaluation)
{
    std::string depth_approx_path = input_path +  db + ".approx_tukey_optimized";
    std::string depth_path = input_path + db + ".tukey";
    DEPTHS approx_depths;
    DEPTHS depths;
    DataIO<int>::ReadTrivialMatrix(depth_path, depths);
    DataIO<int>::ReadTrivialMatrix(depth_approx_path, approx_depths);

    int approximationError = 0;
    int approximationAbsoluteError = 0;

    int min = std::numeric_limits<int>::max();
    int sum_graphs = 0;
    int sum_nodes = 0;
    int sum_depths = 0;
    int max_error = 0;
    int max_error_per_graph = 0;
    for (int i = 0; i < approx_depths.size(); ++i) {
        ++sum_graphs;
        auto const &y = approx_depths[i];
        auto const &z = depths[i];
        int diff = 0;
        int errors_per_graph = 0;
        for (int j = 0; j < y.size(); ++j) {
            int b = y[j];
            int c = z[j];
            ++sum_nodes;
            sum_depths += c;

            approximationError += b - c;
            approximationAbsoluteError += std::abs(b - c);
            diff += std::abs(b - c);
            max_error = std::max(std::abs(b - c), max_error);
            if (std::abs(b - c) != 0)
            {
                errors_per_graph += std::abs(b - c);
            }
        }
        max_error_per_graph = std::max(max_error_per_graph, errors_per_graph);
    }

    double relative_depths = approximationError/(double) sum_depths;
    double relative_per_node = approximationError/(double) sum_nodes;
    double relative_per_graph = approximationError/(double) sum_graphs;

    std::cout << "Estimation Error: " << approximationError << std::endl;
    std::cout << "Error Absolute: " << approximationAbsoluteError << std::endl;
    std::cout << "Relative Depth Error: " << relative_depths << std::endl;
    std::cout << "Relative Node Error: " << relative_per_node << std::endl;
    std::cout << "Relative Graph Error: " << relative_per_graph << std::endl;
    std::cout << "Max Error: " << max_error << std::endl;
    std::cout << "Max Error per Graph: " << max_error_per_graph << std::endl;

    fileEvaluation.headerValueInsert({"Data",
                                              "AbsoluteError",
                                              "RelativeError",
                                              "ErrorPerNode",
                                              "ErrorPerGraph",
                                              "MaxError",
                                              "MaxErrorPerGraph"},
                                     {db,
                                      std::to_string(approximationAbsoluteError),
                                      std::to_string(relative_depths),
                                      std::to_string(relative_per_node),
                                      std::to_string(relative_per_graph),
                                      std::to_string(max_error),
                                      std::to_string(max_error_per_graph)});
}

int main(int argc, char *argv[]) {
    std::vector<std::string> dbs = {"MUTAG", "NCI1", "NCI109", "OHSU", "Peking_1", "PTC_FM", "PTC_FR", "PTC_MM", "PTC_MR", "COIL-DEL", "Cuneiform", "ENZYMES", "KKI", "MSRC_9", "MSRC_21"};
    std::string input_path = "../../Graphs/";
    std::string output_path = "../out/";
    std::string argument;

    for (int i = 0; i < argc; ++i) {
        std::string str_argument = std::string(argv[i]);
        bool str_argument_key = (str_argument[0] == '-');

        if (str_argument_key){
            argument.clear();

            if (std::strcmp(argv[i], "-db") == 0 ) {
                argument = "db";
                dbs.clear();
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
            else if (argument == "input"){
                input_path = std::string(argv[i]);
            }
            else if (argument == "output"){
                output_path = std::string(argv[i]);
            }
        }
    }

    FileEvaluation f = FileEvaluation(output_path, "comparison");

    if (!dbs.empty()) {
        for (auto const &db: dbs) {
            std::cout << "DB: " << db << std::endl;
            compare_tukey(input_path, output_path, db, f);
            std::cout << std::endl << std::endl;
        }
    }
    else if(!input_path.empty()){
        compare_tukey(input_path, output_path, "", f);
        std::cout << std::endl << std::endl;
    }
    f.save(false, true,std::ios_base::out);
}
