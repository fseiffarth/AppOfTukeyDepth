//
// Created by florian on 18.02.22.
//

#ifndef CLOSURES_APPROXIMATIONFUNCTIONS_H
#define CLOSURES_APPROXIMATIONFUNCTIONS_H


#include "../Data/GraphData.h"
#include "../Data/OuterplanarGraphData.h"
#include "../Executables/Experiments/experiment_core_approx.h"

class ApproximationFunctions {

public:
    static void approximate_core(GraphData &graph, std::vector<GraphData> &samples, int generator_size,
                                 CoreApproxApproxParams &params, int seed, std::map<int, int> &degree_distribution,
                                 std::vector<TIntV> &coreNodes, double &runtime);

    static void approximate_core(GraphData& graph, std::vector<OuterplanarGraphData>& samples, int generator_size, CoreApproxApproxParams& params, int seed, std::map<int, int>& degree_distribution, std::vector<TIntV>& coreNodes, double& runtime);

    static void simple_approx(GraphData &graph, int generator_size, int core_iterations, int seed,
                                  std::map<int, int> &degree_distribution, TVec<TInt> &coreNodes, double &runtime,
                                  double time_constraint = -1, int iteration_constraint = 0);
};
#endif //CLOSURES_APPROXIMATIONFUNCTIONS_H
