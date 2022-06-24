//
// Created by florian on 13.05.22.
//

#ifndef TUKEYDEPTH_TUKEYDEPTH_H
#define TUKEYDEPTH_TUKEYDEPTH_H

#include <DataClasses.h>

class TukeyDepth {
public:
    static void run(int id, const GraphStruct& graph, std::vector<int>& depths, int geodesicDistance = -1);
    static void run_parallel(int id, const GraphStruct& graph, std::vector<int>& depths, int geodesicDistance = -1);
};


#endif //TUKEYDEPTH_TUKEYDEPTH_H
