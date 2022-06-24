//
// Created by florian on 18.02.22.
//

#include <iostream>
#include "ApproximationFunctions.h"
#include "../ClosureOperators/GraphClosures.h"
#include "StaticFunctions.h"
#include "../Experiments/DS2021/SetupExperiment.h"

void ApproximationFunctions::simple_approx(GraphData &graph, int generator_size, int core_iterations, int seed, std::map<int, int>& degree_distribution,
                   TIntV& coreNodes, double& runtime, double time_constraint, int iteration_constraint) {
    GraphClosureSP gc;
    ClosureParameters closureParameters;
    std::vector<NodeId> v_intersection;
    std::cout << std::endl;
    std::set<NodeId> overlap;
    runtime = 0;
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < core_iterations; ++i) {
        StaticFunctions::generateInputSet(closureParameters.input_set, graph, generator_size, i + core_iterations * seed);
        closureParameters.iteration_number = iteration_constraint;
        closureParameters.timeConstraint = time_constraint/core_iterations;
        gc.naive_closure(graph, closureParameters);
        std::cout << "\tClosure Size: " << closureParameters.closed_set.size() << std::endl;
        if (i == 0) {
            overlap = closureParameters.closed_set;
        } else {
            v_intersection.clear();
            std::set_intersection(overlap.begin(), overlap.end(),
                                  closureParameters.closed_set.begin(), closureParameters.closed_set.end(),
                                  std::back_inserter(v_intersection));
            overlap.clear();
            if (i == core_iterations -1) {
                closureParameters.closed_set.clear();
                closureParameters.closed_set.insert(v_intersection.begin(), v_intersection.end());
            }
            else {
                for (auto elem: v_intersection) {
                    overlap.insert(elem);
                }
            }
        }
        std::cout << "\tOverlap Size: " << closureParameters.closed_set.size() << std::endl;
    }
    coreNodes.Clr();
    for (auto elem: closureParameters.closed_set) {
        coreNodes.Add(elem);
    }
    //Measure runtime before getting statistics
    runtime = ((double) std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now() - start).count() /
               1000000.0);
}

void ApproximationFunctions::approximate_core(GraphData& graph, std::vector<GraphData>& samples, int generator_size, CoreApproxApproxParams& params, int seed, std::map<int, int>& degree_distribution, std::vector<TIntV>& coreNodes, double& runtime){
    GraphClosureSP gc;
    ClosureParameters closureParameters;
    std::vector<int> overall_approximation = std::vector<int>(graph.size(), 0);
    double closure_time = 0;
    runtime = 0;
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::high_resolution_clock::now();
    std::vector<int> approximation = std::vector<int>(graph.size(), 0);

    std::vector<std::set<NodeId>> coreSets = std::vector<std::set<NodeId>>(params.threshold.size(), std::set<NodeId>());
    for (int i = 0; i < params.coreIterations; ++i) {
        StaticFunctions::generateInputSet(closureParameters.input_set, graph, generator_size, i + params.coreIterations * seed);
        int sample_number = (int) samples.size();
        gc.approx_closure(graph, samples, sample_number, closureParameters, approximation, closure_time, params.save_load_samples);
        if (!params.overall) {
            for (int j = 0; j < params.threshold.size(); ++j) {
                SetupExperiment::getClosedFromApproximation(approximation, closureParameters.closed_set,
                                                            (int) sample_number, params.threshold[j]);

                for (int k = 0; k < params.outer_loop; ++k) {
                    closureParameters.input_set = closureParameters.closed_set;
                    gc.approx_closure(graph, samples, sample_number, closureParameters, approximation,
                                      closure_time,
                                      params.save_load_samples);
                    SetupExperiment::getClosedFromApproximation(approximation, closureParameters.closed_set,
                                                                (int) sample_number,params.threshold[j]);
                }

                std::cout << "\tClosure Size: " << closureParameters.closed_set.size() << std::endl;

                if (i == 0) {
                    coreSets[j] = closureParameters.closed_set;
                } else {
                    //Intersect with previous found
                    auto it1 = coreSets[j].begin();
                    auto it2 = closureParameters.closed_set.begin();
                    while ((it1 != coreSets[j].end()) && (it2 != closureParameters.closed_set.end())) {
                        if (*it1 < *it2) {
                            coreSets[j].erase(it1++);
                        } else if (*it2 < *it1) {
                            ++it2;
                        } else { // *it1 == *it2
                            ++it1;
                            ++it2;
                        }
                    }
                    coreSets[j].erase(it1, coreSets[j].end());
                }
                std::cout << "\tCore Size: " << coreSets[j].size() << std::endl;
            }
        }
        else{
            for(int j = 0; j < approximation.size(); ++j){
                overall_approximation[j] += approximation[j];
            }
        }
    }
    if (params.overall){
        SetupExperiment::getClosedFromApproximation(overall_approximation, closureParameters.closed_set, (int) samples.size(), params.overall_threshold);
        std::cout << "\tOverlap Size: " << closureParameters.closed_set.size() << std::endl;
    }
    coreNodes.clear();
    for(int j = 0; j < coreSets.size(); ++j) {
        coreNodes.emplace_back(TIntV());
        for (auto elem: coreSets[j]) {
            coreNodes[j].Add(elem);
        }
    }
    //Measure runtime before getting statistics
    runtime = ((double) std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now() - start).count() /
               1000000.0);
}

void ApproximationFunctions::approximate_core(GraphData& graph, std::vector<OuterplanarGraphData>& samples, int generator_size, CoreApproxApproxParams& params, int seed, std::map<int, int>& degree_distribution, std::vector<TIntV>& coreNodes, double& runtime){
    GraphClosureSP gc;
    ClosureParameters closureParameters;
    std::vector<int> overall_approximation = std::vector<int>(graph.size(), 0);
    double closure_time = 0;
    runtime = 0;
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::high_resolution_clock::now();
    std::vector<int> approximation = std::vector<int>(graph.size(), 0);

    std::vector<std::set<NodeId>> coreSets = std::vector<std::set<NodeId>>(params.threshold.size(), std::set<NodeId>());
    for (int i = 0; i < params.coreIterations; ++i) {
        StaticFunctions::generateInputSet(closureParameters.input_set, graph, generator_size, i + params.coreIterations * seed);
        int sample_number = (int) samples.size();
        gc.approx_closure(graph, samples, sample_number, closureParameters, approximation, closure_time, params.save_load_samples);
        if (!params.overall) {
            for (int j = 0; j < params.threshold.size(); ++j) {
                SetupExperiment::getClosedFromApproximation(approximation, closureParameters.closed_set,
                                                            (int) sample_number, params.threshold[j]);
                for (int k = 0; k < params.outer_loop; ++k) {
                    closureParameters.input_set = closureParameters.closed_set;
                    gc.approx_closure(graph, samples, sample_number, closureParameters, approximation,
                                      closure_time,
                                      params.save_load_samples);
                    SetupExperiment::getClosedFromApproximation(approximation, closureParameters.closed_set,
                                                                (int) sample_number,params.threshold[j]);
                }

                std::cout << "\tClosure Size: " << closureParameters.closed_set.size() << std::endl;

                if (i == 0) {
                    coreSets[j] = closureParameters.closed_set;
                } else {
                    //Intersect with previous found
                    auto it1 = coreSets[j].begin();
                    auto it2 = closureParameters.closed_set.begin();
                    while ((it1 != coreSets[j].end()) && (it2 != closureParameters.closed_set.end())) {
                        if (*it1 < *it2) {
                            coreSets[j].erase(it1++);
                        } else if (*it2 < *it1) {
                            ++it2;
                        } else { // *it1 == *it2
                            ++it1;
                            ++it2;
                        }
                    }
                    coreSets[j].erase(it1, coreSets[j].end());
                }
                std::cout << "\tCore Size: " << coreSets[j].size() << std::endl;
            }
        }
        else{
            for(int j = 0; j < approximation.size(); ++j){
                overall_approximation[j] += approximation[j];
            }
        }
    }
    if (params.overall){
        SetupExperiment::getClosedFromApproximation(overall_approximation, closureParameters.closed_set, (int) samples.size(), params.overall_threshold);
        std::cout << "\tOverlap Size: " << closureParameters.closed_set.size() << std::endl;
    }
    coreNodes.clear();
    for(int j = 0; j < coreSets.size(); ++j) {
        coreNodes.emplace_back(TIntV());
        for (auto elem: coreSets[j]) {
            coreNodes[j].Add(elem);
        }
    }
    //Measure runtime before getting statistics
    runtime = ((double) std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now() - start).count() /
               1000000.0);
    if (params.save_outerplanar_approx) {
        for(int j = 0; j < coreNodes.size(); ++j) {
            StaticFunctions::save(params.out_path + graph.GetName(), coreNodes[j], "approx_core");
            StaticFunctions::save(params.stripped_path, coreNodes[j],
                                  "approx_core_" + std::to_string((int) (params.threshold[j] * 100)));
        }
    }
}
