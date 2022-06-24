//
// Created by Florian on 12.04.2021.
//

#ifndef CLOSURES_GRAPHDATA_H
#define CLOSURES_GRAPHDATA_H
#include "Data.h"
#include "../Utils/typedefs.h"

enum class GraphType{
    GENERAL,
    TREE,
    OUTERPLANAR,
};

struct GraphDataChangeable {
    GraphDataChangeable()= default;
    explicit GraphDataChangeable(const PUNGraph& graph);
    int Id = 0;
    std::vector<int> distances;
    std::vector<int> containmentList;
    std::vector<std::vector<int>> predecessors;
    std::vector<int> neighborList;
};

class GraphData : public Data<PUNGraph> {
public:
    GraphData();

    explicit GraphData(const std::string& graphPath);
    GraphData(const std::string& graphPath, const std::string& labelPath);
    explicit GraphData(const PUNGraph& graph, const std::string& name = "");
    GraphData(const PUNGraph& graph, const std::string& name, Labels& labels);
    GraphData(const PUNGraph &graph, int size, const std::string& name = "");

    //Copying
    GraphData(const GraphData& other);

    static void Update(const std::string& graphPath);
    void WriteBinary(const std::string& graphPath, bool Labeled = false, bool OnlyGraph = true) const;
    void ReadBinary(const std::string& graphPath);

    [[nodiscard]] PUNGraph get_graph() const;
    PUNGraph graph();
    void set_graph(PUNGraph graph);

    [[nodiscard]] TUNGraph::TNodeI node(NodeId Id) const;
    NodeId elem(NodeId Id) override;
    [[nodiscard]] int degree(NodeId Id) const;
    [[nodiscard]] NodeId neighbor(int NodeId, int NeighborIdx) const;
    [[nodiscard]] size_t size() const override;
    int nodes() const;
    int edges() const;
    double density() const;
    void save(const std::string& path) const;
    void save_dot(const std::string& path) const;
    void save_edges(const std::string& path) const;
    void save_bin(const std::string& path) const;
    [[nodiscard]] GraphData mergeGraphs(const GraphData& other) const;
    void mergeGraphs(const GraphData& other);
    void operator+=(const GraphData& other){ mergeGraphs(other);}
    GraphData operator+(const GraphData& other) const {return mergeGraphs(other);}

    TUNGraph::TNodeI begin() const{return this->get_graph()->BegNI();} ;
    TUNGraph::TNodeI end() const{return this->get_graph()->BegNI();};
    TUNGraph::TNodeI operator++() {return this->get_graph()->BegNI();}

    //
    void getDegreeDistribution(std::map<int, int>& degree_distribution) const;
    void getDegreeDistribution(std::map<int, int>& degree_distribution,const std::set<NodeId>& NodeSet) const;
    void getDegreeDistribution(std::map<int, int>& degree_distribution,const TIntV& NodeSet) const;

    struct NeighborIterator{
        // Prefix increment
        void operator++() { ++_idx;};
        NodeId operator*() const { return _graph->neighbor(_nodeId, _idx); }
        friend bool operator== (const NeighborIterator& a, int b) { return a._idx == b; };
        friend bool operator!= (const NeighborIterator& a, int b) { return a._idx != b; };

        const GraphData* _graph{};
        NodeId _nodeId{};
        int _idx{};
    };
    NeighborIterator begin(NodeId nodeId) { return NeighborIterator{this, nodeId, 0}; }
    [[nodiscard]] int end(NodeId nodeId) const   { return this->get_graph()->GetNI(nodeId).GetDeg(); }


    GraphType graphType = GraphType::GENERAL;
    int maxDegree{};
    GraphDataChangeable graphDataInfo;

    void print() const;


    void init(int size);
    void init();

    void UpdateSingletons(const std::string &graphPath);
};




#endif //CLOSURES_GRAPHDATA_H
