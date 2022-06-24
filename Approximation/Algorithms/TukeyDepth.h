//
// Created by Florian on 21.04.2021.
//

#ifndef CLOSURES_TUKEYDEPTH_H
#define CLOSURES_TUKEYDEPTH_H

#include "../Utils/typedefs.h"
#include "../Data/Data.h"
#include "../Data/GraphData.h"

enum class TDAlgorithm{
    ALTERNATIVE_TUKEY,
    TUKEY_APPROX,
    TUKEY_APPROX_OPTIMIZED,
    EXACT,
};

struct ClosureParameters;

template <typename C, typename I>
class TukeyDepth {
public:
    TukeyDepth(std::vector<GraphData> &data, C& closure, I& initUpdate);

    void run(int threads = 1, int seed = 0, TDAlgorithm algorithm = TDAlgorithm::ALTERNATIVE_TUKEY);
    std::vector<std::vector<int>> approximated_depth{};
    int datasize();
    void print();

    static void GetWeakTukeyDepth(std::vector<GraphData>& graphs, std::vector<std::vector<int>> &nodeFeatures, int threads, int seed, TDAlgorithm wtdAlgorithm = TDAlgorithm::ALTERNATIVE_TUKEY);



        private:
    std::vector<GraphData>& _data;
    C _closure;
    I _initUpdate;


    void one_sided_run(int seed);

    void print_single(int i);
    void test_core(GraphData& graph, std::vector<int>& depth);

    void alternative_tukey(std::mt19937_64& gen);

    void optimized_algorithm(std::mt19937_64 &gen);
    void optimized_algorithm_two_sides(int seed);

    void exact();
    void shift_depths(int i);
private:
    void run_optimized_algorithm_two_sides(std::mt19937_64& gen, GraphData &graph, TUNGraph::TNodeI& nodeIt, int j, std::vector<int> &node_distances,
                                           std::vector<int> &neighbor_distances, int &Id, std::vector<int> &validityA,
                                           std::vector<int> &validityB, ClosureParameters& closureParametersA, ClosureParameters& closureParametersB);
};
#include "TukeyDepth.txx"

#endif //CLOSURES_TUKEYDEPTH_H