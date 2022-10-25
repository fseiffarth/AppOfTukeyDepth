//
// Created by florian on 13.05.22.
//

#ifndef TUKEYDEPTH_TUKEYDEPTH_H
#define TUKEYDEPTH_TUKEYDEPTH_H

#include <DataClasses.h>

class TukeyDepth {
public:
    static void run(Indices id, const GraphStruct& graph, std::vector<Indices>& depths, Indices geodesicDistance = -1);
    static void run_parallel(Indices id, const GraphStruct& graph, std::vector<Indices>& depths, Indices num_threads, Indices geodesicDistance = -1);
};


#endif //TUKEYDEPTH_TUKEYDEPTH_H
