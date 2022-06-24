//
// Created by Florian on 16.05.2021.
//

#include "InitUpdateBase.h"

template<typename T>
void InitUpdateGreedy<T>::init(T& data, std::mt19937_64& gen, std::vector<std::set<NodeId>> &trainingSets, std::vector<NodeId> &unlabeledElements) {
    this->_tags.init(data.size(), trainingSets.size());
    this->_partialOrder.init(data.size(), trainingSets);
    this->_partialOrder.initOrder(1);
    for (NodeId nodeId: unlabeledElements) {
        this->_partialOrder.insert(0, nodeId);
    }
}

template<typename T>
void InitUpdateGreedy<T>::update(T& data, std::set<NodeId>& addedObjects, NodeId currentElement,
                                 std::vector<int>& invalidExtensions) {
    this->_partialOrder.removeElements(addedObjects);
}

template<typename T>
InitUpdateGreedy<T>::InitUpdateGreedy(std::string name) : InitUpdate<T>(name) {}

template<typename T>
InitUpdateGreedyRandom<T>::InitUpdateGreedyRandom(std::string name) : InitUpdateGreedy<T>(name){}

template<typename T>
void InitUpdateGreedyRandom<T>::init(T& data, std::mt19937_64& gen, std::vector<std::set<NodeId>> &trainingSets, std::vector<NodeId> &unlabeledElements) {
    this->_tags.init(data.size(), trainingSets.size());
    this->_tags.shuffle(gen);
    this->_partialOrder.init(data.size(), trainingSets);
    this->_partialOrder.initOrder(1);
    std::vector<int> classes;
    for (int i = 0; i < trainingSets.size(); ++i) {
        classes.template emplace_back(i);
    }
    for (NodeId nodeId: unlabeledElements) {
        this->_partialOrder.insert(0, nodeId);
    }
}

template<typename T>
bool InitUpdate<T>::nextClass(int& nextClass, const NodeId element) {
    if (this->_tags.nextClass(element, nextClass)) {
        return true;
    }
    return false;
}

template<typename T>
bool InitUpdate<T>::next(NodeId &nextElement, std::mt19937_64 &gen) {
    bool valid = _partialOrder.next(nextElement, gen);
    currentElement = nextElement;
    return valid;
}

