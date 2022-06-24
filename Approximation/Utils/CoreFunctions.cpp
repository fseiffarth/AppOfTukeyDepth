//
// Created by florian on 28.04.22.
//

#include <vector>
#include <queue>
#include "CoreFunctions.h"

void CoreFunctions::GetCoreDegreeDistribution(PUNGraph core, std::map<int, int> &degreeMap) {
    for (auto NodeIt = core->BegNI(); NodeIt != core->EndNI(); NodeIt++) {
        if (degreeMap.find(NodeIt.GetDeg()) != degreeMap.end()){
            degreeMap[NodeIt.GetDeg()]++;
        }
        else{
            degreeMap.insert({NodeIt.GetDeg(), 1});
        }
    }
}

void CoreFunctions::OrderByNodeContraction(PUNGraph graph, std::map<int, int> &nodeOrderMap) {
    std::deque<int> degree2Nodes;
    int level = 0;
    for (auto NodeIt = graph->BegNI(); NodeIt != graph->EndNI(); NodeIt++) {
        if (NodeIt.GetDeg() == 2){
            degree2Nodes.emplace_back(NodeIt.GetId());
        }
    }
    int lastNode = degree2Nodes.back();
    std::vector<int> neighbors;
    nodeOrderMap.insert({0,0});
    bool newLevel = false;
    while (!degree2Nodes.empty()){
        int x = degree2Nodes.front();
        degree2Nodes.pop_front();
        if (x == lastNode){
            newLevel = true;
            lastNode = degree2Nodes.back();
        }
        neighbors.clear();
        for (int i = 0; i < graph->GetNI(x).GetDeg(); ++i) {
            neighbors.emplace_back(graph->GetNI(x).GetNbrNId(i));
        }

        if (neighbors.size() <= 2){
            ++nodeOrderMap[level];
            for (auto i : neighbors) {
                graph->DelEdge(x, i);
            }
            if (neighbors.size() == 2) {
                graph->AddEdge(neighbors[0], neighbors[1]);
            }

            if (graph->GetNI(neighbors[0]).GetDeg() == 2){
                degree2Nodes.emplace_back(neighbors[0]);
            }
            if (graph->GetNI(neighbors[1]).GetDeg() == 2){
                degree2Nodes.emplace_back(neighbors[1]);
            }
            if (newLevel) {
                newLevel = false;
                ++level;
                nodeOrderMap.insert({level, 0});
            }
        }
        else{
            throw std::range_error("Wrong neighbor size!");
        }
    }
    int numNodes = 0;
    for (auto NodeIt = graph->BegNI(); NodeIt != graph->EndNI(); NodeIt++) {
        if (NodeIt.GetDeg() >= 1){
            ++numNodes;
        }
    }
    nodeOrderMap[level] = numNodes;

}
