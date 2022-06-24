//
// Created by florian on 12.04.22.
//

#ifndef CLOSURES_LOADTUDORTMUNDGRAPHDATA_H
#define CLOSURES_LOADTUDORTMUNDGRAPHDATA_H


#include <string>
#include "../Data/GraphData.h"

class LoadTUDortmundGraphData {
public:
    static void GraphDataToGraphList(const std::string& path, const std::string& dbName, std::vector<GraphData>& graphs, std::vector<int>& graphLabels, std::vector<std::vector<int>>* graphsEdgeLabels= nullptr, std::vector<std::vector<int>>* graphsNodeAttributes= nullptr, std::vector<std::vector<int>>* graphsEdgeAttributes= nullptr);
};


#endif //CLOSURES_LOADTUDORTMUNDGRAPHDATA_H
