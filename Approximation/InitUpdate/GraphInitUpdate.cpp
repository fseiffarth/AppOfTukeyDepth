//
// Created by Florian on 16.05.2021.
//

#include "GraphInitUpdate.h"
#include "../Utils/typedefs.h"

bool comparator ( const std::pair<int, int>& l, const std::pair<int, int>& r){ return l.second < r.second; }
bool distancesComparator(const std::pair<int, std::vector<std::pair<int, int>>>& l, const std::pair<int, std::vector<std::pair<int, int>>>& r){return l.second.back().second > r.second.back().second;}

void
GraphInitUpdateFarthestPoint::init(GraphData &data, std::mt19937_64 &gen, std::vector<std::set<NodeId>> &trainingSets,
                                   std::vector<NodeId> &unlabeledElements) {
    this->_tags.init(data.size(), trainingSets.size());
    this->_partialOrder.init(data.size(), trainingSets);
    this->_distances = std::vector<std::pair<int, std::vector<std::pair<int, int>>>>(data.size(), std::pair<int, std::vector<std::pair<int, int>>>{0, std::vector<std::pair<int, int>>(trainingSets.size(), std::pair<int, int>{0, INT_MAX})});
    int counter = 0;
    for (auto &[key, value] : this->_distances) {
        key = counter;
        ++counter;
        int classCounter = 0;
        for (auto &vd : value) {
            vd.first = classCounter;
            ++classCounter;
        }
    }
    int classId = 0;
    for (auto const& classNodes : trainingSets) {
        for (NodeId nodeId : classNodes) {
            TIntV distanceVector;
            TSnap::GetShortestDistances(data.get_graph(), nodeId, true, true, distanceVector);
            for(NodeId unlabeledNodeId : unlabeledElements){
                if (getDistance(unlabeledNodeId, classId)  > distanceVector[unlabeledNodeId]){
                    setDistance(unlabeledNodeId, classId, distanceVector[unlabeledNodeId]);
                }
            }
        }
        ++classId;
    }
    //Update based on the distance
    int nodeId = 0;
    for (auto& distanceVectors : this->_distances) {
        std::shuffle(distanceVectors.second.begin(), distanceVectors.second.end(), gen);
        std::stable_sort(distanceVectors.second.begin(), distanceVectors.second.end(), comparator);
        int idx = 0;
        for (auto&  [classId, distance] : distanceVectors.second) {
            this->_tags.set(nodeId, classId, idx);
            ++idx;
        }
        ++nodeId;
    }
    //Sort distance vector
    std::shuffle(_distances.begin(), _distances.end(), gen);
    std::stable_sort(_distances.begin(), _distances.end(), distancesComparator);
    int oldValue = -1;
    int uniqueValues = -1;
    for (auto const &[key, value] : this->_distances) {
        if (value[0].second != INT_MAX) {
            if (value[0].second != oldValue) {
                ++uniqueValues;
                this->_partialOrder.push_back(key);
                oldValue = value[0].second;
            } else {
                this->_partialOrder.insert(uniqueValues, key);
            }
        }
    }
}

void GraphInitUpdateFarthestPoint::update(GraphData& data, std::set<NodeId> &addedObjects, NodeId currentElement, std::vector<int> &invalidExtensions) {
    this->_partialOrder.removeElements(addedObjects);
}

int GraphInitUpdateFarthestPoint::getDistance(NodeId nodeId, int classId) {
    return this->_distances[nodeId].second[classId].second;
}

void GraphInitUpdateFarthestPoint::setDistance(NodeId nodeId, int classId, int distance) {
    this->_distances[nodeId].second[classId].second = distance;
}

void
GraphInitUpdateNearestNeighbor::init(GraphData &data, std::mt19937_64 &gen, std::vector<std::set<NodeId>> &trainingSets,
                                     std::vector<NodeId> &unlabeledElements) {
    this->_tags.init((int) data.size(), (int) trainingSets.size());
    this->_tags.shuffle(gen);
    this->_partialOrder.init((int) data.size(), trainingSets);
    this->_partialOrder.initOrder(1);
    this->_neighbors.clear();
    int classId = 0;
    for(auto const& set : trainingSets){
        for (NodeId  nodeId : set) {
            auto node = data.get_graph()->GetNI(nodeId);
            for (int i = 0; i < node.GetDeg(); ++i) {
                int neighborId = node.GetNbrNId(i);
                if (this->_neighbors.find(neighborId) != this->_neighbors.end()) {
                    this->_neighbors[neighborId].emplace_back(classId);
                } else {
                    this->_neighbors.insert({neighborId, {classId}});
                }
            }
        }
        ++classId;
    }
    for (auto& [key, val] : this->_neighbors) {
        if (this->_partialOrder.getOrder(key) != PartialOrder::REMOVED) {
            std::shuffle(val.begin(), val.end(), gen);
            this->_partialOrder.insert(0, key);
            for (int cId : val) {
                this->_tags.moveTagByClass(key, cId, 0);
            }
        }
    }
}

void GraphInitUpdateNearestNeighbor::update(GraphData& data, std::set<NodeId> &addedObjects, NodeId currentElement, std::vector<int> &invalidExtensions) {
    //Update partial order
    for (NodeId nodeId : addedObjects) {
        TUNGraph::TNodeI node = data.get_graph()->GetNI(nodeId);
        int degree = node.GetDeg();
        for (int i = 0; i < degree; ++i) {
            int neighborId = node.GetNbrNId(i);
            if (this->_partialOrder.notInOrder(neighborId)) {
                auto itPair = this->_tags.removedTags(nodeId);
                for (auto it = itPair.first; it != itPair.second; ++it) {
                    this->_tags.removeTagByClass(neighborId, *it);
                }
                if (this->_tags.hasValidTags(neighborId)) {
                    this->_partialOrder.insert(this->_partialOrder.getOrder(nodeId) + 1, neighborId);
                }
            }
        }
    }
    this->_partialOrder.removeElements(addedObjects);
    //Update tags
    for (auto const& invalidExtension : invalidExtensions) {
        TUNGraph::TNodeI node = data.get_graph()->GetNI(currentElement);
        int degree = node.GetDeg();
        for (int i = 0; i < degree; ++i) {
            int neighborId = node.GetNbrNId(i);
            if(this->_partialOrder.getOrder(neighborId) == -1 || this->_partialOrder.getOrder(neighborId) > this->_partialOrder.getCurrentOrder()){
                this->_tags.removeTagByClass(neighborId, invalidExtension);
            }
        }
    }
}
