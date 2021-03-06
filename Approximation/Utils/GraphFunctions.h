//
// Created by florian on 08.11.21.
//

#ifndef CLOSURES_GRAPHFUNCTIONS_H
#define CLOSURES_GRAPHFUNCTIONS_H


#include "typedefs.h"
#include "../Data/GraphData.h"
#include "GraphStructs.h"
#include "directed_graph.h"
#include "FileEvaluation.h"
#include "Enums.h"
#include "OuterplanarSubgraphDFS.h"

class OuterplanarComponent;

struct GraphParams {
    //Paths
    std::string in_path = "../../GraphData/";
    int max_nodes = std::numeric_limits<int>::max();
    int max_edges = std::numeric_limits<int>::max();
    int min_nodes = 0;
    int min_edges = 0;
};

struct CoreCalcParams{
    //Paths
    std::string in_path = "../../GraphData/";
    std::string out_path = "../out/Experiments/Overlap/";

    int thread_num = 4;

    int max_nodes = std::numeric_limits<int>::max();
    int max_edges = std::numeric_limits<int>::max();
    int min_nodes = 0;
    int min_edges = 0;

    int generator_size = 10;
    int generator_seeds = 0;
    int coreIterations = 3;
    int sample_number = 100;
};

struct CoreApproxApproxParams{
    //Paths
    std::string in_path = "../../GraphData/";
    std::string out_path = "../out/KDD/";
    std::string eval_name = "kdd_2022";

    int thread_num = 4;

    int max_nodes = std::numeric_limits<int>::max();
    int max_edges = std::numeric_limits<int>::max();
    int min_nodes = 0;
    int min_edges = 0;

    std::vector<int> generator_size = {10};
    std::vector<int> generator_seeds = {0};
    int coreIterations = 3;
    int sample_number = 100;
    std::vector<double> threshold = {1.0 / (double) this->sample_number};

    int outer_loop = 0;
    double overall_threshold = 1.0 / (double) this->sample_number;
    bool overall = false;
    int sample_seed = 32487643;
    bool outerplanar_new = true;

    //Eval
    bool periphery = false;
    bool core_iteration = false;
    bool exact = true;
    bool approx_core = true;

    bool simple_approx = false;
    int runtime = 0;
    int simple_approx_iterations = 0;

    bool core_stats = false;
    bool outerplanar_statistics = false;
    bool threshold_percentage = false;
    bool tree_eval = true;
    bool outer_fixed_point = false;
    bool save_load_samples = false;
    bool small_graphs = true;
    bool large_graphs = false;
    bool save_outerplanar_approx = true;
    bool delete_samples = true;

    //inline variables
    std::string stripped_path;
};

class GraphFunctions {
public:
    static bool SmallGraph(const GraphData& graph){
        return graph.nodes() < 335000 && graph.edges() < 1950000;
    }

    std::vector<NodeId> nodesOnAllShortestPaths(const GraphData &graphData, NodeId A, NodeId B);

    static void mergeGraphs(PUNGraph &graph, const PUNGraph &other);

    static void addEdgesToGraph(PUNGraph &graph, const PUNGraph &other);

    static void
    getNodesInBall(const PUNGraph &undirectedGraph, TIntV &reachedNodes, std::mt19937_64 &gen, int size = -1,
                   NodeId rootNodeId = -1, int hop = -1);

    static void bfsTree(const PUNGraph &undirectedGraph, const PUNGraph &subTree, NodeId rootNodeId,
                        std::vector<NodePair> &missingEdges, std::mt19937_64 &gen);

    template<typename T>
    static PUNGraph bfsSubtree(GraphData &graph, std::vector<NodeId> &neighborIds, T &closure, std::mt19937_64 &gen,
                               bool basedOnTraining);

    static void bfsSubtree(const PUNGraph &undirectedGraph, PUNGraph &subtree, std::vector<NodeId> &neighborIds,
                           std::mt19937_64 &gen, int rootNodeId = -1);

    static void bfsSubtree(const PUNGraph &undirectedGraph, GraphData &subtreeData, std::vector<NodeId> &neighborIds,
                           std::mt19937_64 &gen, int rootNodeId = -1);


    static void
    bfsSubtree(const PUNGraph &graph, std::vector<NodePair> &edges, NodeId roodNodeId, std::mt19937_64 &gen);

    static void bfsRandomSubtree(const PUNGraph &graph, std::vector<NodePair> &edges, std::mt19937_64 &gen);

    /// Returns the distances in a graph given a root node
    /// \param graph
    /// \param root
    /// \param distances
    static void BFSDistances(const PUNGraph &graph, NodeId root,std::vector<int> &distances);

    static void
    dfsSubtree(const PUNGraph &undirectedGraph, GraphData &subTree, std::vector<NodeId> &neighborIds,
               std::mt19937_64 &gen,
               int rootNodeId = -1);

    static int getMaxDegree(const PUNGraph &graph);

    static void generateNeighborVector(const PUNGraph &graph, std::vector<int> &neighborVector);

    static void
    dfsTree(const PUNGraph &undirectedGraph, std::vector<NodePair> &edges, NodeId rootNodeId, std::mt19937_64 &gen);

    static void dfsRandomSubtree(const PUNGraph &undirectedGraph, std::vector<NodePair> &edges, std::mt19937_64 &gen);

    static PUNGraph randomTree(int size, int seed);

    static PUNGraph randomTree(int size, std::mt19937_64 &gen);

    static PUNGraph randomOuterplanar(int size, int seed);

    static bool IsTree(const PUNGraph &graph);

    static bool IsOuterPlanar(const PUNGraph &graph, NodeId src=-1, NodeId dst=-1);

    static bool
    IsMaximalOuterplanarSubgraph(const PUNGraph &graph, const PUNGraph &subgraph, std::vector<NodePair> &missingEdges);

    static bool IsOuterPlanarIncremental(const PUNGraph &graph, const NodePair &newEdge, std::vector<int> &nodeDegrees,
                                         std::vector<int> &degree2Nodes);

    static PUNGraph
    GetReducedGraph(const PUNGraph &graph, std::vector<Nodes> &reducedComponents, bool renumberNodes = false);

    static PUNGraph RebuildReducedComponents(const PUNGraph &fullGraph, const PUNGraph &reducedGraph,
                                             std::vector<Nodes> &reducedComponents, bool renumberNodes = false);

    static PUNGraph ResetGraphIds(const PUNGraph &graph);

    static void NormalizeGraphIds(PUNGraph &graph);

    static void NormalizeIdsOfGraphs(const std::vector<std::string> &paths);

    static bool FoundInComponent(NodeId nodeId, std::vector<Nodes> &components, int &componentIdx);

    static PUNGraph dfsRandomTree(const PUNGraph &graph, std::vector<NodeId> &visitedNodes, std::mt19937_64 &gen);

    template<typename T>
    static PUNGraph
    maximalOuterplanarSubgraphGreedy(GraphData &graph, T &closure, std::mt19937_64 &gen, bool basedOnTraining);

    static PUNGraph maximalOuterplanarSubgraphGreedy(const PUNGraph &graph, std::mt19937_64 &gen);

    static PUNGraph maximalOuterplanarSubgraphGreedy(const GraphData &graph, std::mt19937_64 &gen);

    template<typename T>
    static PUNGraph outerplanarSubgraphLinear(GraphData &graph, T &closure, std::mt19937_64 &gen, bool basedOnTraining);

    static PUNGraph outerplanarSubgraphLinear(const PUNGraph &graph, std::mt19937_64 &gen);

    static PUNGraph outerplanarSubgraphLinear(const GraphData &graph, std::mt19937_64 &gen);

    static PUNGraph maximalOuterplanarSubgraphBFSGreedy(const PUNGraph &graph, std::mt19937_64 &gen);

    static PUNGraph maximalOuterplanarSubgraphBFSGreedyIncremental(const PUNGraph &graph, std::mt19937_64 &gen);

    static PUNGraph maximalOuterplanarSubgraphBFSGreedy(const GraphData &graph, std::mt19937_64 &gen);

    static PUNGraph maximalOuterplanarSubgraphDFSGreedy(const PUNGraph &graph, std::mt19937_64 &gen);


    static PUNGraph maximalOuterplanarSubgraphLinear(const PUNGraph &graph, std::mt19937_64 &gen);

    static PUNGraph maximalOuterplanarSubgraphLinear(const GraphData &graph, std::mt19937_64 &gen);

    template<typename T>
    static PUNGraph
    maximalOuterplanarSubgraphLinear(GraphData &graph, T &closure, std::mt19937_64 &gen, bool basedOnTraining);


    static void GetBiconnectedOuterplanarFaces(const PUNGraph &component, OuterplanarComponent& outerplanarComponent);
    static void GetBiconnectedOuterplanarFaceNum(const PUNGraph &component, int &face_num);

    static void
    biconnectedComponentMaximalOuterPlanarSubgraph(const PUNGraph &graph, TCnCom &component, PUNGraph &outerplanarGraph,
                                                   std::mt19937_64 &gen);

    static void GetBiconnectedComponents(const PUNGraph &graph, std::vector<PUNGraph> &graphComponents);

    static void
    deleteEdges(std::unordered_map<NodePair, bool, hashNodePair> &resultingEdges, const std::vector<NodePair> &edges);

    static void GetCoreGraphStats(GraphData &graph, TIntV &coreNodes, double runtime, const std::string& core_name = "Core", FileEvaluation* fileEvaluation = nullptr);

    static void print(const PUNGraph &graph, const std::string &name);

    static void get_core(GraphData& graph, int generator_size, int core_iterations, int seed, std::vector<int>& intersection_loss, TIntV& coreNodes, double& runtime, double& avg_preclosure_steps, bool print=false);
    static void get_core(GraphData &graph, int generator_size, int core_iterations, int seed, TIntV& coreNodes, bool print=false);
    static void get_graph_paths(GraphParams& params, std::vector<std::tuple<std::string, int>>& paths_sizes);
    static void get_graph_paths(CoreApproxApproxParams &params, std::vector<std::tuple<std::string, int>> &paths_sizes);
    static void get_graph_paths(CoreCalcParams &params, std::vector<std::tuple<std::string, int>> &paths_sizes);
    static void get_graph_size(const std::string &path, int& nodes, int& edges);


    struct EdgeInfo {
    public:
        EdgeInfo() = default;

        EdgeInfo(int triangulationCount, bool validEdge, bool triangulationEdge = false, bool deletedEdge = false)
                : _triangulationCount(triangulationCount), _valid(validEdge), _triangulationEdge(triangulationEdge),
                  _deleted(deletedEdge) {};

        int is_valid() const { return _valid; };

        void set_valid(bool isValid) { _valid = isValid; };

        void delete_edge() {
            _deleted = true;
            _valid = false;
            _triangulationCount = 0;
        };

        bool is_deleted() const { return _deleted; };

        int get_count() const { return _triangulationCount; };

        int &count() { return _triangulationCount; };

        bool is_triangulationEdge() const { return _triangulationEdge; };

        bool is_bfs_edge() const { return _bfs_edge; };

        void set_bfs_edge() { _bfs_edge = true; };
        std::pair<std::vector<NodePair> *, std::vector<NodePair> *> dependentEdgesPointer = std::pair<std::vector<NodePair> *, std::vector<NodePair> *>{
                nullptr, nullptr};
    private:
        int _triangulationCount = 0;
        bool _valid = true;
        bool _triangulationEdge = false;
        bool _deleted = false;
        bool _bfs_edge = false;
    };

    static void
    getValidNeighbor(PUNGraph &graph, std::unordered_map<NodePair, EdgeInfo, hashNodePair> &algorithmEdges, int nodeId,
                     int neighborIdx, NodeId &neighborId);

    static void reduceCountByOne(std::unordered_map<NodePair, bool, hashNodePair> &resultingEdges, EdgeInfo &edgeInfo,
                                 std::mt19937_64 &gen);

    static int deleteEdges(std::unordered_map<NodePair, EdgeInfo, hashNodePair> &algorithmEdges, EdgeInfo &currentEdge);

    void reduceCountByOne(std::unordered_map<NodePair, bool, hashNodePair> &resultingEdges, EdgeInfo &edgeInfo);

    static void
    deleteAndReduceCount(std::unordered_map<NodePair, EdgeInfo, hashNodePair> &algorithmEdges, EdgeInfo &FirstEdge,
                         EdgeInfo &SecondEdge, EdgeInfo &TriangleEdge);


    static void generateNeighborVector(const SIMPLE_GRAPH &graph, std::vector<int> &neighborVector);

    static int getMaxDegree(const SIMPLE_GRAPH &graph);

    static void GetLargestComponent(GraphData &data);

    static void analyse_graph(const PUNGraph &graph, const std::string &name = "", bool component_analysis = false,
                              FileEvaluation *fileEvaluation = nullptr);

    static PUNGraph RemoveSubgraph(const PUNGraph &baseGraph, const PUNGraph &subGraph);

    static void getDegreeDistribution(const PUNGraph &graph, std::map<int, int> &degree_distribution,
                                      std::vector<NodeId> *nodes = nullptr);

    static void addRandomEdges(GraphData &data, int edgeNum, int seed);

    static void
    checkingOuterplanarity(const PUNGraph &graph, const PUNGraph &outerplanarSubgraph, int &notOuterplanarSubgraphs,
                           int &notMaximalSubgraphs, std::vector<int> &nonOuterplanarSeeds,
                           std::vector<int> &nonMaximalSeeds, std::vector<double> &algorithmMissingEdges,
                           std::vector<double> &maximalEdges, int seed = 0);

    static void GetSamples(GraphData &graph, PatternType type, std::vector<GraphData> &subgraphs,
                           OuterplanarSubgraphDFS *outerplanarSubgraphDFS, std::vector<NodeId>& neighborIds, int samples, int seed, double& runtime, bool save_samples = false, bool p = false);
    static void GetOuterplanarSamples(GraphData &graph, PatternType type, std::vector<OuterplanarGraphData> &subgraphs,
                           OuterplanarSubgraphDFS *outerplanarSubgraphDFS, int samples, int seed, double& runtime, double &conversion_runtime, bool clear = true, bool save_samples = false, bool p = false);

};




#endif //CLOSURES_GRAPHFUNCTIONS_H
