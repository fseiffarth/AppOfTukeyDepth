//
// Created by florian on 28.04.22.
//

#ifndef CLOSURES_COREFUNCTIONS_H
#define CLOSURES_COREFUNCTIONS_H


#include <Snap.h>
#include <map>

class CoreFunctions {
public:
    static void GetCoreDegreeDistribution(PUNGraph core, std::map<int, int>& degreeMap);

    static void OrderByNodeContraction(PUNGraph graph, std::map<int, int> &nodeOrderMap);
};


#endif //CLOSURES_COREFUNCTIONS_H
