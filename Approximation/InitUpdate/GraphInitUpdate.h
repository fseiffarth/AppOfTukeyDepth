//
// Created by Florian on 16.05.2021.
//

#ifndef CLOSURES_GRAPHINITUPDATE_H
#define CLOSURES_GRAPHINITUPDATE_H


#include "../Data/GraphData.h"
#include "InitUpdateBase.h"

class GraphInitUpdateFarthestPoint : public InitUpdate<GraphData>{
public:
    explicit GraphInitUpdateFarthestPoint(std::string name, bool nearestClassRandom = true) : InitUpdate<GraphData>(name), _nearestClassRandom(nearestClassRandom){};
    void init(GraphData &data, std::mt19937_64 &gen, std::vector<std::set<NodeId>> &trainingSets, std::vector<NodeId> &unlabeledElements) override;
    void update(GraphData& data, std::set<NodeId> &addedObjects, NodeId currentElement, std::vector<int> &invalidExtensions) override;

private:
    bool _nearestClassRandom;
    std::vector<std::pair<int, std::vector<std::pair<int, int>>>> _distances;
    int getDistance(NodeId nodeId, int classId);
    void setDistance(NodeId nodeId, int classId, int distance);
};

class GraphInitUpdateNearestNeighbor : public InitUpdate<GraphData>{
public:
    explicit GraphInitUpdateNearestNeighbor(const std::string& name = "", bool nearestClassRandom = true, int threshold = -1) : InitUpdate<GraphData>(name), _nearestClassRandom(nearestClassRandom), _threshold(threshold){};
    void init(GraphData &data, std::mt19937_64 &gen, std::vector<std::set<NodeId>> &trainingSets, std::vector<NodeId> &unlabeledElements) override;
    void update(GraphData& data, std::set<NodeId> &addedObjects, NodeId currentElement, std::vector<int> &invalidExtensions) override;

private:
    int _threshold;
    bool _nearestClassRandom;
    std::map<int, std::vector<int>> _neighbors;
};

class GraphInitUpdateNearestNeighborByClass : public InitUpdate<GraphData>{
public:
    explicit GraphInitUpdateNearestNeighborByClass(const std::string& name = "", const std::vector<int>& classList = std::vector<int>(), int threshold = -1) : InitUpdate<GraphData>(name), _classList(classList), _threshold(threshold){};
    void init(GraphData &data, std::mt19937_64 &gen, std::vector<std::set<NodeId>> &trainingSets, std::vector<NodeId> &unlabeledElements) override;
    void update(GraphData& data, std::set<NodeId> &addedObjects, NodeId currentElement, std::vector<int> &invalidExtensions) override;

private:
    int _threshold;
    std::vector<int> _classList;
    std::map<int, std::vector<int>> _neighbors;
};

#endif //CLOSURES_GRAPHINITUPDATE_H
