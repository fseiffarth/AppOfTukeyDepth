//
// Created by Florian on 16.05.2021.
//

#ifndef CLOSURES_INITUPDATEBASE_H
#define CLOSURES_INITUPDATEBASE_H

#include "../Utils/typedefs.h"
#include "../Data/Data.h"

struct PartialOrder{
public:
    bool next(NodeId& nextElement, std::mt19937_64& gen);
    void removeElements(const std::set<NodeId>& elements);
    void init(int size, std::vector<std::set<NodeId>>& trainingSets);
    void initOrder(int num);
    void insert(int orderId, NodeId nodeId);
    void push_back(NodeId nodeId);
    void add(NodeId nodeId);
    int size();
    bool getElementOrder(NodeId nodeId, int& idx);
    bool getNextOrderValue();
    int getOrder(NodeId nodeId);
    bool notInOrder(NodeId nodeId){return this->_idToOrder[nodeId] == -1;}
    bool removedFromOrder(NodeId nodeId){return this->_idToOrder[nodeId] == -2;};
    int getCurrentOrder(){return minOrderValue;};
    static const int REMOVED = -2;
private:
    std::vector<std::set<NodeId>> _order;
    int minOrderValue = 0;
    std::vector<int> _idToOrder;

};

struct Tags{
    void init(int dataSize, int classNumber);
    void swapTagsByClass(NodeId nodeId, int classA, int classB);
    void swapTagsByIdx(NodeId nodeId, int idxA, int idxB);
    void moveTagByClass(NodeId nodeId, int classId, int newIdx = 0);
    void moveTagByIdx(NodeId nodeId, int idx, int newIdx = 0);
    void sortAllTags();
    void shuffle(std::mt19937_64& gen);
    void removeTagByIdx(NodeId nodeId, int idx);
    void removeTagByClass(NodeId nodeId, int classId);
    bool nextClass(NodeId nodeId, int& classId);
    void set(NodeId nodeId, int classId, int idx);
    std::pair<std::vector<int>::iterator, std::vector<int>::iterator> removedTags(NodeId nodeId);
    bool hasValidTags(NodeId nodeId);

private:
    std::vector<std::vector<int>> tagOrder;
    std::vector<int> currentTagIdx;
};

template <typename T>
class InitUpdate {
public:
    explicit InitUpdate(std::string name) : _name(name){};
    virtual void init(T& data, std::mt19937_64& gen, std::vector<std::set<NodeId>>& trainingSets, std::vector<NodeId>& unlabeledElements) = 0;
    virtual void update(T& data, std::set<NodeId> &addedObjects, NodeId currentElement, std::vector<int> &invalidExtensions) = 0;
    bool nextClass(int& nextClass, NodeId element);
    bool next(NodeId& nextElement, std::mt19937_64& gen);
    std::string getName(){return _name;};

protected:
    //std::vector<std::vector<int>> _v;
    Tags _tags;
    PartialOrder _partialOrder;
    NodeId currentElement{};
    std::string _name;
};

template <typename T>
class InitUpdateGreedy : public InitUpdate<T> {
public:
    explicit InitUpdateGreedy(std::string name);
    void init(T& data,std::mt19937_64& gen, std::vector<std::set<NodeId>>& trainingSets, std::vector<NodeId>& unlabeledElements) override;
    void update(T& data, std::set<NodeId>& addedObjects, NodeId currentElement, std::vector<int>& invalidExtensions) override;
};

template <typename T>
class InitUpdateGreedyRandom : public InitUpdateGreedy<T> {
public:
    explicit InitUpdateGreedyRandom(std::string name);
    void init(T& data,std::mt19937_64& gen, std::vector<std::set<NodeId>>& trainingSets, std::vector<NodeId>& unlabeledElements) override;
};

#include "InitUpdateBase.tpp"


#endif //CLOSURES_INITUPDATEBASE_H
