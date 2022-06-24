//
// Created by Florian on 21.04.2021.
//

#include "TukeyDepth.h"
#include "MCSS.h"
#include "../Utils/GraphFunctions.h"
#include "../InitUpdate/InitUpdateBase.h"


template <typename C, typename I>
TukeyDepth<C, I>::TukeyDepth::TukeyDepth(std::vector<GraphData> &data, C& closure, I& initUpdate) : _data(data), _closure(closure), _initUpdate(initUpdate) {
    for (int i = 0; i < data.size(); ++i) {
        this->approximated_depth.template emplace_back(std::vector<int>());
        this->approximated_depth[i].resize(_data[i].nodes());
        std::fill(this->approximated_depth[i].begin(), this->approximated_depth[i].end(), 0);
    }
}


template<typename C, typename I>
void TukeyDepth<C, I>::run(int threads, int seed, TDAlgorithm algorithm) {
    omp_set_num_threads(threads);
    std::mt19937_64 gen(seed);
    std::cout << "Depths: " << std::endl;
    switch(algorithm){
        case TDAlgorithm::ALTERNATIVE_TUKEY:
            alternative_tukey(gen);
            break;
        case TDAlgorithm::TUKEY_APPROX:
            one_sided_run(seed);
            break;
        case TDAlgorithm::TUKEY_APPROX_OPTIMIZED:
            optimized_algorithm_two_sides(seed);
            break;
        case TDAlgorithm::EXACT:
            exact();
            break;
    }
    int i = 0;
}

template<typename C, typename I>
int TukeyDepth<C, I>::datasize() {
    return _data.size();
}

template<typename C, typename I>
void TukeyDepth<C, I>::one_sided_run(int seed) {
#pragma omp parallel for firstprivate(_closure, _initUpdate, seed) shared(approximated_depth, _data) default(none)
    for (int i = 0; i < _data.size(); ++i) {
        std::mt19937_64 gen(seed + i);
        auto& graph = _data[i];
        for (int n = 0; n < graph.size(); ++n) {
            const auto &Node = graph.get_graph()->GetNI(n);
            NodeId nodeId = Node.GetId();
            for (int j = 0; j < Node.GetDeg(); ++j) {
                MCSS mcss = MCSS(graph, _closure, _initUpdate);
                NodeId neighbor = Node.GetNbrNId(j);
                std::vector<std::set<NodeId>> input_set = {{nodeId},{neighbor}};
                mcss.run(input_set, gen);
                const std::vector<ClosureParameters> &out = mcss.get_output();

#pragma omp critical
                for (int k = 0; k < graph.size(); ++k) {
                    if (out[0].closed_set.find(k) == out[0].closed_set.end()){
                        approximated_depth[i][k] = std::max(approximated_depth[i][k], (int) out[0].closed_set.size());
                    }
                    else if (out[1].closed_set.find(k) == out[1].closed_set.end()){
                        approximated_depth[i][k] = std::max(approximated_depth[i][k], (int) out[1].closed_set.size());
                    } else{
                        throw std::runtime_error("Invalid run!");
                    }
                }
            }
        }
#pragma omp critical
        {
            shift_depths(i);
            print_single(i);
        }
        //test_core(graph, approximated_depth[i]);
    }
}

template<typename C, typename I>
void TukeyDepth<C, I>::print() {
    std::cout << "Depths: " << std::endl;
    int counter = 0;
    for (auto& x : this->approximated_depth) {
        ++counter;
        std:: cout << "Graph " << counter << ": ";
        for (auto i : x) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
    }
}

template<typename C, typename I>
void TukeyDepth<C, I>::print_single(int i) {
    std::stringstream ss;
    std::map<int, std::vector<int>, std::greater<int>> depthDistribution;
    for (int j=0; j < approximated_depth[i].size(); ++j) {
        int depth = approximated_depth[i][j];
        if (depthDistribution.find(depth) == depthDistribution.end()){
            depthDistribution.insert({depth, std::vector<int>(1, j)});
        }
        else{
            depthDistribution[depth].template emplace_back(j);
        }
    }

    ss << std::endl << "Graph " << i << ": " << std::endl;
    ss << "\t\tDepth distribution: ";
    ss << "{";
    for (auto& [key, val]:depthDistribution) {
        ss << key << ":" << val.size() << " ";
    }
    ss << "}" << std::endl;
    ss << "\t\tDepths by Node: ";
    for (auto a: approximated_depth[i]) {
        ss << a << " ";
    }
    StaticFunctionsLib::PrintStream(ss);
}

template<typename C, typename I>
void TukeyDepth<C, I>::test_core(GraphData &graph, std::vector<int>& depth) {
    TIntV coreNodes;
    GraphFunctions::get_core(graph, 5, 3, 0, coreNodes);

    std::map<int, std::vector<int>, std::greater<int>> depthDistribution;
    for (int j=0; j < coreNodes.Len(); ++j) {
        int node = coreNodes[j];
        int d = depth[coreNodes[j]];
        if (depthDistribution.find(d) == depthDistribution.end()){
            depthDistribution.insert({d, std::vector<int>(1, node)});
        }
        else{
            depthDistribution[d].template emplace_back(node);
        }
    }

    std::cout << "\t\tDepth distribution: ";
    std::cout << "{";
    for (auto& [key, val]:depthDistribution) {
        std::cout << key << ":" << val.size() << " ";
    }
    std::cout << "} ";
    std::cout << " Core: " << "(Size: " << coreNodes.Len() << ") ";
    for (int i = 0; i < coreNodes.Len(); ++i) {
        std::cout << coreNodes[i] + 1 << "(" << depth[coreNodes[i]] << ") ";
    }
    std::cout << std::endl;
}

template<typename C, typename I>
void TukeyDepth<C, I>::alternative_tukey(std::mt19937_64& gen) {
#pragma omp parallel for firstprivate(gen, _closure, _initUpdate) shared(_data, approximated_depth) default(none)
    for (int i = 0; i < _data.size(); ++i) {
        auto &data = _data[i];
        MCSS mcss = MCSS(data, _closure, _initUpdate);
        std::vector<ClosureParameters> out;
        std::vector<std::set<NodeId>> input_set;
        for (TUNGraph::TNodeI Node = data.get_graph()->BegNI(); Node != data.get_graph()->EndNI(); Node++) {
            for (int j = 0; j < Node.GetDeg(); ++j) {
                NodeId Neighbor = Node.GetNbrNId(j);
                input_set = {{Node.GetId()},
                             {Neighbor}};
                mcss.run(input_set, gen);
                out = mcss.get_output();
#pragma omp critical
                {
                    for (auto &set: out) {
                        for (auto elem: set.closed_set) {
                            approximated_depth[i][elem] = std::min(approximated_depth[i][elem],(int) set.closed_set.size());
                        }
                    }
                }
            }
        }
#pragma omp critical
        {
            print_single(i);
        }
    }
}

template<typename C, typename I>
void TukeyDepth<C, I>::optimized_algorithm(std::mt19937_64 &gen) {
#pragma omp parallel for firstprivate(gen, _closure, _initUpdate) shared(_data, approximated_depth) default(none)
    //Iterate over all graphs
    for (int i = 0; i < _data.size(); ++i) {
        auto& graph = _data[i];
        std::vector<int> depths = std::vector<int>(graph.size(), _data[i].nodes());
        int Id = 0;
        std::vector<ClosureParameters> out;
        std::vector<int> validity = std::vector<int>(graph.size(), Id); //Checks validity and element is valid iff validity[elementId] != Id
        std::vector<int> node_distances;
        std::vector<int> neighbor_distances;
        for (TUNGraph::TNodeI nodeIt = graph.get_graph()->BegNI(); nodeIt != graph.get_graph()->EndNI(); nodeIt++) {
            int degree = nodeIt.GetDeg();
            NodeId nodeId = nodeIt.GetId();
            GraphFunctions::BFSDistances(graph.get_graph(), nodeId, node_distances);
            for (int j = 0; j < degree; ++j) {
                ++Id;
                NodeId neighbor = nodeIt.GetNbrNId(j);
                validity[nodeId] = Id;
                validity[neighbor] = Id;
                GraphFunctions::BFSDistances(graph.get_graph(), neighbor, neighbor_distances);

                for (int k = 0; k < graph.size(); ++k) {
                    if (node_distances[k] + 1 == neighbor_distances[k]){
                        validity[k] = Id;
                    }
                }

                ClosureParameters closureParameters;
                closureParameters.closed_set = {neighbor};
                std::set<int> forbidden = {nodeIt.GetId()};
                closureParameters.forbidden_elements = {&forbidden};
                GraphClosureSP cl;

                std::vector<int> elementCandidates = std::vector<int>(graph.size(), 0);
                std::iota(elementCandidates.begin(), elementCandidates.end(), 0);

                while(!elementCandidates.empty()){
                    int idx = std::uniform_int_distribution<int>(0, (int) elementCandidates.size() - 1)(gen);
                    int randomNode = elementCandidates[idx];
                    std::swap(elementCandidates[idx], elementCandidates.back());
                    elementCandidates.pop_back();
                    if (validity[randomNode] != Id && closureParameters.closed_set.find(randomNode) == closureParameters.closed_set.end()){
                        bool success = cl.incremental_closure(graph, closureParameters, randomNode);
                        if (!success){
                            closureParameters.closed_set = closureParameters.input_set;
                            for (int k = 0; k < graph.get_graph()->GetNI(randomNode).GetDeg(); ++k) {
                                int currentNodeNeighbor = graph.get_graph()->GetNI(randomNode).GetNbrNId(k);
                                if (neighbor_distances[currentNodeNeighbor] == neighbor_distances[randomNode] + 1){
                                    validity[currentNodeNeighbor] = Id;
                                }
                            }
                        }
                    }
                }
#pragma omp critical
                for (int k = 0; k < graph.size(); ++k) {
                    if (closureParameters.closed_set.find(k) == closureParameters.closed_set.end()) {
                        approximated_depth[i][k] = std::max(approximated_depth[i][k], (int) closureParameters.closed_set.size());
                    }
                }
            }
        }
#pragma omp critical
        {
            shift_depths(i);
            print_single(i);
        }
        //test_core(graph, approximated_depth[i]);
    }
}

template<typename C, typename I>
void TukeyDepth<C, I>::run_optimized_algorithm_two_sides(std::mt19937_64& gen, GraphData& graph, TUNGraph::TNodeI& nodeIt, int j, std::vector<int>& node_distances, std::vector<int>& neighbor_distances, int& Id, std::vector<int>& validityA, std::vector<int>& validityB, ClosureParameters& closureParametersA, ClosureParameters& closureParametersB){
        int nodeId = nodeIt.GetId();
        ++Id;
        NodeId neighbor = nodeIt.GetNbrNId(j);
        validityA[nodeId] = Id;
        validityA[neighbor] = Id;

        validityB[nodeId] = Id;
        validityB[neighbor] = Id;

        GraphFunctions::BFSDistances(graph.get_graph(), neighbor, neighbor_distances);

        for (int k = 0; k < graph.size(); ++k) {
            if (node_distances[k] + 1 == neighbor_distances[k]){
                validityA[k] = Id;
            }
            else if (node_distances[k] == neighbor_distances[k] + 1){
                validityB[k] = Id;
            }
        }

        closureParametersA.closed_set = {neighbor};
        closureParametersA.input_set = {neighbor};

        closureParametersB.closed_set = {nodeId};
        closureParametersB.input_set = {nodeId};

        closureParametersA.forbidden_elements = {&closureParametersB.closed_set};
        closureParametersB.forbidden_elements = {&closureParametersA.closed_set};


        GraphClosureSP cl;

        std::vector<int> elementCandidates = std::vector<int>(graph.size(), 0);
        std::iota(elementCandidates.begin(), elementCandidates.end(), 0);

        while(!elementCandidates.empty()){
            int idx = std::uniform_int_distribution<int>(0, (int) elementCandidates.size() - 1)(gen);
            int randomNode = elementCandidates[idx];
            std::swap(elementCandidates[idx], elementCandidates.back());
            elementCandidates.pop_back();
            if (validityA[randomNode] != Id){
                bool success = cl.incremental_closure(graph, closureParametersA, randomNode);
                if (success){
                    for (auto x:closureParametersA.added_elements) {
                        validityA[x] = Id;
                        validityB[x] = Id;
                        closureParametersA.input_set.insert(x);
                    }
                }
                else {
                    closureParametersA.closed_set = closureParametersA.input_set;
                    for (int k = 0; k < graph.get_graph()->GetNI(randomNode).GetDeg(); ++k) {
                        int currentNodeNeighbor = graph.get_graph()->GetNI(randomNode).GetNbrNId(k);
                        if (neighbor_distances[currentNodeNeighbor] == neighbor_distances[randomNode] + 1) {
                            validityA[currentNodeNeighbor] = Id;
                        }
                    }
                    if (validityB[randomNode] != Id) {
                        success = cl.incremental_closure(graph, closureParametersB, randomNode);
                        if (success) {
                            for (auto x: closureParametersB.added_elements) {
                                validityA[x] = Id;
                                validityB[x] = Id;
                                closureParametersB.input_set.insert(x);
                            }
                        } else {
                            closureParametersB.closed_set = closureParametersB.input_set;
                            for (int k = 0; k < graph.get_graph()->GetNI(randomNode).GetDeg(); ++k) {
                                int currentNodeNeighbor = graph.get_graph()->GetNI(randomNode).GetNbrNId(k);
                                if (node_distances[currentNodeNeighbor] == node_distances[randomNode] + 1) {
                                    validityB[currentNodeNeighbor] = Id;
                                }
                            }
                        }
                    }
                }
            }
            if (validityB[randomNode] != Id){
                bool success = cl.incremental_closure(graph, closureParametersB, randomNode);
                if (success){
                    for (auto x:closureParametersB.added_elements) {
                        validityA[x] = Id;
                        validityB[x] = Id;
                        closureParametersB.input_set.insert(x);
                    }
                }
                else{
                    closureParametersB.closed_set = closureParametersB.input_set;
                    for (int k = 0; k < graph.get_graph()->GetNI(randomNode).GetDeg(); ++k) {
                        int currentNodeNeighbor = graph.get_graph()->GetNI(randomNode).GetNbrNId(k);
                        if (node_distances[currentNodeNeighbor] == node_distances[randomNode] + 1){
                            validityB[currentNodeNeighbor] = Id;
                        }
                    }
                }
            }
            validityA[randomNode] = Id;
            validityB[randomNode] = Id;
        }
}

template<typename C, typename I>
void TukeyDepth<C, I>::optimized_algorithm_two_sides(int seed) {
#pragma omp parallel for firstprivate(seed, _closure, _initUpdate) shared(_data, approximated_depth) default(none)
    //Iterate over all graphs
    for (int i = 0; i < _data.size(); ++i) {
        std::mt19937_64 gen(seed + i);
        auto& graph = _data[i];
        std::vector<int> depths = std::vector<int>(graph.size(), _data[i].nodes());

            int Id = 0;
            std::vector<int> validityA = std::vector<int>(graph.size(),
                                                          Id); //Checks validity and element is valid iff validity[elementId] != Id
            std::vector<int> validityB = std::vector<int>(graph.size(),
                                                          Id); //Checks validity and element is valid iff validity[elementId] != Id
            std::vector<int> node_distances;
            std::vector<int> neighbor_distances;
            for (TUNGraph::TNodeI nodeIt = graph.get_graph()->BegNI(); nodeIt != graph.get_graph()->EndNI(); nodeIt++) {
                NodeId nodeId = nodeIt.GetId();
                GraphFunctions::BFSDistances(graph.get_graph(), nodeId, node_distances);
                int degree = nodeIt.GetDeg();
                for (int j = 0; j < degree; ++j) {
                    ClosureParameters closureParametersA;
                    ClosureParameters closureParametersB;
                    run_optimized_algorithm_two_sides(gen, graph, nodeIt, j, node_distances, neighbor_distances,
                                                      Id, validityA, validityB, closureParametersA, closureParametersB);
#pragma omp critical
                    for (int k = 0; k < graph.size(); ++k) {
                        if (closureParametersA.closed_set.find(k) == closureParametersA.closed_set.end()) {
                            approximated_depth[i][k] = std::max(approximated_depth[i][k],
                                                                (int) closureParametersA.closed_set.size());
                        }
                        if (closureParametersB.closed_set.find(k) == closureParametersB.closed_set.end()) {
                            approximated_depth[i][k] = std::max(approximated_depth[i][k],
                                                                (int) closureParametersB.closed_set.size());
                        }
                    }
                }
            }
#pragma omp critical
        {
            shift_depths(i);
            print_single(i);
        }
        //test_core(graph, approximated_depth[i]);
    }
}


template<typename C, typename I>
void TukeyDepth<C, I>::exact() {

}

template<typename C, typename I>
void TukeyDepth<C, I>::shift_depths(int i) {
    for (int j = 0; j < _data[i].size(); ++j) {
        approximated_depth[i][j] = _data[i].size() - approximated_depth[i][j];
    }
    ++i;
}

void GetWeakTukeyDepth(std::vector<GraphData>& graphs, std::vector<std::vector<int>> &nodeFeatures, int threads, int seed, TDAlgorithm wtdAlgorithm) {
    omp_set_num_threads(std::min(threads, omp_get_num_threads()));
    GraphClosureSP closure;
    InitUpdateGreedy<GraphData> init_update = InitUpdateGreedy<GraphData>("");
    TukeyDepth wtd = TukeyDepth<GraphClosureSP, InitUpdateGreedy<GraphData>>(graphs, closure, init_update);
    double mean_size = 0;
    double mean_edges = 0;
    for (const auto& graph:graphs) {
        mean_size += (double) graph.size()/(double) graphs.size();
        mean_edges += (double) graph.edges()/(double) graphs.size();
    }
    std::cout << "Calculate the weak tukey depth for " << graphs.size() << " graphs." << " Mean size: " << mean_size << " Mean edges: " << mean_edges << std::endl;
    wtd.run(threads, seed, wtdAlgorithm);
    int maxDepth = 0;
    for (auto const & vector : wtd.approximated_depth) {
        maxDepth = std::max(maxDepth, *std::max_element(vector.begin(), vector.end()));
    }
    std::cout << std::endl << "Max Depth: " << maxDepth << std::endl;
    nodeFeatures = wtd.approximated_depth;
}

