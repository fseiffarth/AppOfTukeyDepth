//
// Created by Florian on 13.04.2021.
//

#ifndef CLOSURES_STATICFUNCTIONS_H
#define CLOSURES_STATICFUNCTIONS_H


#include <string>
#include <map>
#include <numeric>
#include <algorithm>
#include "typedefs.h"
#include "../Data/GraphData.h"
#include "iostream"

class GraphParams;




class StaticFunctions {
public:
    static std::set<NodeId> getNodesWithLabel(Labels& labels, Label targetLabel);

    static void saveValuesToFile(const std::string& path, const std::vector<std::string>& header, const std::vector<std::string>& values, std::_Ios_Openmode mode = std::ios_base::app);
    static void headerValueInsert(std::vector<std::string>& header, std::vector<std::string>&values, const std::vector<std::string>& new_header, const std::vector<std::string>& new_values);
    static void generateInputSet(std::set<NodeId>& input_set, const GraphData& graphData, int input_size, int seed);
    static void printComponents(TCnComV& components);
    static void save(const std::string& path,const TIntV& NodeIds, const std::string& extension = "core");
    static void load(const std::string &path, std::vector<int> &NodeIds);
    static void load(const std::string &path, TIntV &NodeIds);

    static void LoadGraphDatabase(std::string& databaseName, std::string& graphPath, std::vector<GraphData>& graphs, std::string* labelPath = nullptr);
};

#endif //CLOSURES_STATICFUNCTIONS_H
