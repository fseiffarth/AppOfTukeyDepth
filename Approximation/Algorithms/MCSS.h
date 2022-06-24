//
// Created by Florian on 08.05.2021.
//

#ifndef CLOSURES_MCSS_H
#define CLOSURES_MCSS_H

#include "../Data/Data.h"
#include "../ClosureOperators/BaseOperator.h"
#include "../Data/GraphData.h"
#include "../ClosureOperators/GraphClosures.h"
#include "../Utils/StaticFunctions.h"
#include <iostream>

template <typename T1, typename T2, typename T3>
class MCSS {
public:
    MCSS(T1 &data, T2& closure, T3& initUpdate);
    bool run(std::vector<std::set<NodeId>>& inputSets, std::mt19937_64& gen);
    Labels predict(std::vector<std::set<NodeId>>& inputSets, std::mt19937_64& gen);
    void print();
    std::vector<ClosureParameters>& get_output();

private:
    void reset();
    T1& _data;
    T2& _closure;
    T3& _initUpdate;
    int queryNum = 0;
    std::vector<int> consideredElements;
    std::vector<std::pair<NodeId, int>> extendedElements;
    std::vector<std::pair<NodeId, int>> queries;
    std::vector<std::set<NodeId>> input;
    std::vector<std::set<NodeId>> closedInput;
    std::vector<std::set<NodeId>> output;
    std::vector<ClosureParameters> closureOutputs;
    bool validRun = true;
};
#include "MCSS.txx"
#endif //CLOSURES_MCSS_H
