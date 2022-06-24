//
// Created by Florian on 08.05.2021.
//

#include <iostream>
#include "MCSS.h"
#include <StaticFunctions.h>

template <typename T1, typename T2, typename T3>
MCSS<T1, T2, T3>::MCSS(T1 &data, T2& closure, T3& initUpdate) : _data(data), _closure(closure), _initUpdate(initUpdate) {}

template <typename T1, typename T2, typename T3>
bool MCSS<T1, T2, T3>::run(std::vector<std::set<NodeId>> &inputSets, std::mt19937_64& gen) {
    this->reset();
    //Preprocessing on input sets
    for (auto & inputSet : inputSets) {
        this->input.template emplace_back(inputSet);
        this->output.template emplace_back(inputSet);
    }
    for (auto& inputSet : this->output) {
        ClosureParameters closureOutput = ClosureParameters(inputSet);
        closureOutputs.emplace_back(closureOutput);
    }
    for (int i = 0; i < this->output.size(); ++i) {
        for (int j = 0; j < closureOutputs.size(); ++j) {
            if (i != j){
                closureOutputs[j].forbidden_elements.push_back(&(this->output[i]));
            }
        }
    }
    //Calculate initial closures
    for (ClosureParameters& closureOutput : closureOutputs) {
        _closure.closure(_data, closureOutput);
        if (closureOutput.output_intersects_forbidden){
            this->validRun = false;
            return false;
        }
    }
    int Counter = 0;
    for (auto& closureOutput : closureOutputs) {
        this->output[Counter].insert(closureOutput.added_elements.begin(), closureOutput.added_elements.end());
        this->closedInput.template emplace_back(this->output[Counter]);
        ++Counter;
    }

    //Get all elements not in some closures
    std::vector<NodeId> elements;
    bool found = false;
    for (NodeId id = 0; id < this->_data.size(); ++id) {
        found = false;
        for (auto & closed : this->closedInput) {
            if (closed.find(id) !=closed.end()){
                found = true;
                break;
            }
        }
        if (!found){
            elements.emplace_back(id);
        }
    }

    //Initialize settings
    this->_initUpdate.init(this->_data, gen, this->closedInput, elements);

    //Start algorithm
    NodeId currentElement;
    int currentClass;
    std::vector<int> invalidExtensions;
    while (this->_initUpdate.next(currentElement, gen)){
        this->consideredElements.emplace_back(currentElement);
        while(this->_initUpdate.nextClass(currentClass, currentElement)){
            invalidExtensions.clear();
            ClosureParameters& currentClosure = this->closureOutputs[currentClass];
            currentClosure.element_to_add = currentElement;
            _closure.closure(_data, currentClosure);
            this->queries.template emplace_back(std::pair<NodeId, int>{currentElement, currentClass});
            ++queryNum;
            if (currentClosure.output_intersects_forbidden){
                currentClosure.reset();
                currentClosure.closed_set = this->output[currentClass];
                invalidExtensions.emplace_back(currentClass);
            }
            else{
                currentClosure.input_set = currentClosure.closed_set;
                this->extendedElements.emplace_back(std::pair<NodeId, int>{currentElement, currentClass});
                this->output[currentClass].insert(currentClosure.added_elements.begin(), currentClosure.added_elements.end());
                this->_initUpdate.update(_data, currentClosure.added_elements, currentElement, invalidExtensions);
                break;
            }
        }

    }
    this->validRun = true;
    return true;
}

template <typename T1, typename T2, typename T3>
void MCSS<T1, T2, T3>::print() {
    std::string validRunString = this->validRun ? "Yes" : "No";
    std::cout << "Valid Run: " << validRunString << std::endl;
    for (std::set<NodeId>& inputSet : this->input) {
        std::cout << "Input Set: " << StaticFunctionsLib::print<std::set<NodeId>, NodeId>(inputSet) << std::endl;
    }
    for (std::set<NodeId> outputSet:this->output) {
        std::cout << "Closed Set: " << StaticFunctionsLib::print<std::set<NodeId>, NodeId>(outputSet) << std::endl;
    }
    std::cout << "Query Number: " << queryNum << std::endl;
    std::cout << "Considered Elements: " << StaticFunctionsLib::print<std::vector<NodeId>, NodeId>(this->consideredElements) << std::endl;
    std::cout << "Extended Elements: " << StaticFunctionsLib::print<std::vector<std::pair<NodeId, int>>, std::pair<NodeId, int>>(this->extendedElements, true) << std::endl;
    std::cout << "Queries: " << StaticFunctionsLib::print<std::vector<std::pair<NodeId, int>>, std::pair<NodeId, int>>(this->queries, true) << std::endl;
}

template <typename T1, typename T2, typename T3>
Labels MCSS<T1, T2, T3>::predict(std::vector<std::set<NodeId>> &inputSets, std::mt19937_64 &gen) {
    Labels labels = Labels(this->_data.size(), -1);
    run(inputSets, gen);
    int label = 0;
    for (std::set<NodeId>& out : this->output) {
        for(NodeId nodeId: out){
            labels[nodeId] = label;
        }
        ++label;
    }
    return labels;
}

template<typename T1, typename T2, typename T3>
void MCSS<T1, T2, T3>::reset() {
    this->queryNum = 0;
    this->consideredElements.clear();
    this->extendedElements.clear();
    this->queries.clear();
    this->input.clear();
    this->closedInput.clear();
    this->closureOutputs.clear();
    this->output.clear();
}

template<typename T1, typename T2, typename T3>
std::vector<ClosureParameters> &MCSS<T1, T2, T3>::get_output() {
    return this->closureOutputs;
}


