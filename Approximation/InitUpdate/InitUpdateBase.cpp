//
// Created by Florian on 16.05.2021.
//

#include "InitUpdateBase.h"



bool PartialOrder::next(NodeId& nextElement, std::mt19937_64& gen) {
    if (minOrderValue >= 0 && _order.size() > minOrderValue && !_order[minOrderValue].empty())
    {
        int randIdx = std::uniform_int_distribution<int>(0, static_cast<int>(_order[minOrderValue].size()) - 1)(gen);
        nextElement = _order[minOrderValue].extract(std::next(_order[minOrderValue].begin(), randIdx)).value();
        return true;
    }
    nextElement = -1;
    return false;
}

bool PartialOrder::getNextOrderValue() {
    for (int i = minOrderValue + 1; i < this->_order.size(); ++i) {
        if (!this->_order[i].empty()){
            this->minOrderValue = i;
            return true;
        }
    }
    minOrderValue = -1;
    return false;
}

void PartialOrder::initOrder(int num) {
    this->_order = std::vector<std::set<NodeId>>(num, std::set<NodeId>());
}

void PartialOrder::insert(int orderId, NodeId nodeId) {
    if (this->_idToOrder[nodeId] == -1) {
        if (this->_order.size() == orderId) {
            this->push_back(nodeId);
        } else if (orderId >= 0 && orderId < this->_order.size()) {
            this->_order[orderId].insert(nodeId);
            this->_idToOrder[nodeId] = orderId;
        }
    }
}

void PartialOrder::push_back(NodeId nodeId) {
    if (this->_idToOrder[nodeId] == -1) {
        this->_idToOrder[nodeId] = static_cast<int>(this->_order.size());
        this->_order.push_back({nodeId});
    }
}

int PartialOrder::size() {
    return static_cast<int>(this->_order.size());
}

bool PartialOrder::getElementOrder(NodeId nodeId, int& idx) {
    idx = this->_idToOrder[nodeId];
    return idx >= 0;
}

void PartialOrder::removeElements(const std::set<NodeId>& elements) {
    for (const NodeId& element : elements) {
        int idx;
        if (this->getElementOrder(element, idx)) {
            this->_order[idx].extract(element);
            this->_idToOrder[element] = REMOVED;
        }
    }
    if (this->_order[minOrderValue].empty()){
        this->getNextOrderValue();
    }
}

void PartialOrder::init(int size, std::vector<std::set<NodeId>>& trainingSets) {
    this->minOrderValue = 0;
    this->_order.clear();
    if (this->_idToOrder.size() != size){
        this->_idToOrder = std::vector<int>(size, -1);
    }
    else{
        std::fill(this->_idToOrder.begin(), this->_idToOrder.end(), -1);
    }
    for (auto const& set : trainingSets) {
        for(NodeId nodeId : set){
            this->_idToOrder[nodeId] = PartialOrder::REMOVED;
        }
    }
}

void PartialOrder::add(NodeId nodeId) {
    if (this->_idToOrder[nodeId] != PartialOrder::REMOVED) {
        this->_idToOrder[nodeId] = static_cast<int>(this->_order.size()) - 1;
        this->_order.back().insert(nodeId);
    }
}

int PartialOrder::getOrder(NodeId nodeId) {
    return this->_idToOrder[nodeId];
}

void Tags::init(int dataSize, int classNum) {
    if (this->tagOrder.size() != dataSize || this->tagOrder[0].size() != classNum) {
        this->tagOrder = std::vector<std::vector<int>>(dataSize, std::vector<int>(classNum));
        int counter = 0;
        for (auto & tag : this->tagOrder) {
            std::iota(tag.begin(), tag.end(), 0);
            ++counter;
        }
    }
    else{
        this->sortAllTags();
    }
    if (this->currentTagIdx.size() != dataSize) {
        this->currentTagIdx = std::vector<int>(dataSize, -1);
    }
}

void Tags::swapTagsByClass(NodeId nodeId, int classA, int classB) {
    int idxA =  std::find(this->tagOrder[nodeId].begin(), this->tagOrder[nodeId].end(), classA) - this->tagOrder[nodeId].begin();
    int idxB = std::find(this->tagOrder[nodeId].begin(), this->tagOrder[nodeId].end(), classB) - this->tagOrder[nodeId].begin();
    Tags::swapTagsByIdx(nodeId, idxA, idxB);
}

void Tags::swapTagsByIdx(NodeId nodeId, int idxA, int idxB) {
    std::swap(this->tagOrder[nodeId][idxA], this->tagOrder[nodeId][idxB]);
}

void Tags::sortAllTags() {
    int counter = 0;
    for (auto &v : this->tagOrder) {
        std::sort(v.begin(), v.end());
        this->currentTagIdx[counter] = -1;
        ++counter;
    }
}

void Tags::removeTagByIdx(NodeId nodeId, int idx) {
    int currentIndex = this->currentTagIdx[nodeId] - 1;
    if (currentIndex < idx){
        std::rotate(this->tagOrder[nodeId].begin() + currentIndex, this->tagOrder[nodeId].begin() + idx - 1, this->tagOrder[nodeId].begin()  + idx);
        ++currentTagIdx[nodeId];
    }else if(currentIndex == idx){
        ++currentTagIdx[nodeId];
    }
}

void Tags::removeTagByClass(NodeId nodeId, int classId) {
    int classIdx = std::find(this->tagOrder[nodeId].begin(), this->tagOrder[nodeId].end(), classId) - this->tagOrder[nodeId].begin();
    Tags::removeTagByIdx(nodeId, classIdx);
}

void Tags::shuffle(std::mt19937_64& gen) {
    for (auto &v : this->tagOrder) {
        std::shuffle(v.begin(), v.end(), gen);
    }
}

void Tags::moveTagByIdx(NodeId nodeId, int idx, int newIdx) {
    if (idx > newIdx) {
        std::rotate(this->tagOrder[nodeId].begin() + newIdx, this->tagOrder[nodeId].begin() + idx,
                    this->tagOrder[nodeId].begin() + idx + 1);
    }
    else if(idx < newIdx){
        std::rotate(this->tagOrder[nodeId].begin() + idx, this->tagOrder[nodeId].begin() + 1, this->tagOrder[nodeId].begin() + newIdx);
    }
}

void Tags::moveTagByClass(NodeId nodeId, int classId, int newIdx) {
    int classIdx = std::find(this->tagOrder[nodeId].begin(), this->tagOrder[nodeId].end(), classId) - this->tagOrder[nodeId].begin();
    moveTagByIdx(nodeId, classIdx, newIdx);
}

bool Tags::nextClass(NodeId nodeId, int& classId) {
    int a = this->currentTagIdx[nodeId];
    int b = static_cast<int>(this->tagOrder[nodeId].size()) - 1;
    if (a < b){
        ++this->currentTagIdx[nodeId];
        classId = this->tagOrder[nodeId][this->currentTagIdx[nodeId]];
        return true;
    }
    return false;
}

void Tags::set(NodeId nodeId, int classId, int idx) {
    this->tagOrder[nodeId][idx] = classId;
}

std::pair<std::vector<int>::iterator, std::vector<int>::iterator> Tags::removedTags(NodeId nodeId) {
    return {this->tagOrder[nodeId].begin(), this->tagOrder[nodeId].begin() + this->currentTagIdx[nodeId]};
}

bool Tags::hasValidTags(NodeId nodeId) {
    int a = this->currentTagIdx[nodeId];
    int b = static_cast<int>(this->tagOrder[nodeId].size()) - 1;
    if (a < b){
        return true;
    }
    return false;
}
