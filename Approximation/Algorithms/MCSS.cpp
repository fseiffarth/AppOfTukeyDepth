//
// Created by Florian on 08.05.2021.
//

#include <iostream>
#include "MCSS.h"
#include "../Utils/StaticFunctions.h"

MCSS::MCSS(GraphData &data, GraphClosureSP &closure) : _data(data), _closure(closure) {

}
bool MCSS::run(std::vector<std::set<NodeId>> &inputSets, std::mt19937_64& gen) {
    this->queryNum = 0;
    this->consideredElements.clear();
    this->extendedElements.clear();
    this->output.clear();

    //Preprocessing on input sets
    for (auto & inputSet : inputSets) {
        this->input.push_back(inputSet);
    }
    //Preprocessing on input sets
    for (auto & inputSet : inputSets) {
        this->output.push_back(inputSet);
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
        ++Counter;
    }

    //Start algorithm
    std::vector<NodeId> elements;
    bool found = false;
    for (NodeId id = 0; id < this->_data.size(); ++id) {
        found = false;
        for (auto & closureOutput : closureOutputs) {
            if (closureOutput.closed_set.find(id) != closureOutput.closed_set.end()){
                found = true;
                break;
            }
        }
        if (!found){
            elements.emplace_back(id);
        }
    }

    while (!elements.empty()){
        int random_elem = std::uniform_int_distribution<int>(0, elements.size() - 1)(gen);
        NodeId currentElement = elements[random_elem];
        this->consideredElements.template emplace_back(currentElement);
        std::swap(elements[random_elem], elements.back());
        elements.pop_back();
        int counter = 0;
        for (auto& closureOutput : closureOutputs) {
            closureOutput.element_to_add = currentElement;
            _closure.closure(_data, closureOutput);
            if (closureOutput.output_intersects_forbidden){
                closureOutput.reset();
                closureOutput.closed_set = this->output[counter];
            }
            else{
                this->extendedElements.template emplace_back(currentElement);
                this->output[counter].insert(closureOutput.added_elements.begin(), closureOutput.added_elements.end());
                break;
            }
            counter++;
        }

    }
    this->validRun = true;
    return true;
}

void MCSS::print() {
    std::string validRunString = this->validRun ? "Yes" : "No";
    std::cout << "Valid Run: " << validRunString << std::endl;
    for (std::set<NodeId>& inputSet : this->input) {
        std::cout << "Input Set: " << StaticFunctions::print<std::set<NodeId>, NodeId>(inputSet) << std::endl;
    }
    for (std::set<NodeId> outputSet:this->output) {
        std::cout << "Closed Set: " << StaticFunctions::print<std::set<NodeId>, NodeId>(outputSet) << std::endl;
    }
}

Labels MCSS::predict(std::vector<std::set<NodeId>> &inputSets, std::mt19937_64 &gen) {
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


