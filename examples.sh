#!/bin/bash
#Script to run all examples
#run depth examples
./TukeyDepth/build/TukeyDepth -i Graphs/ -o out/ -db MUTAG Cuneiform -t 24
./TukeyDepth/build/TukeyDepth -i Graphs/KarateClub.edges -o out/ -t 24
#run depth approx examples
./Approximation/build/ApproximateTukeyDepth -i Graphs/ -o out/ -db MUTAG Cuneiform -t 24
./Approximation/build/ApproximateTukeyDepth -i Graphs/KarateClub.edges -o out/ -t 24
#run core examples
./Approximation/build/ComputeCore -i Graphs/KarateClub.edges -o out/ -t 24
./Approximation/build/ComputeCore -i Graphs/ -o out/ -db MUTAG Cuneiform -t 24
#evaluate approximation and exact depth
./Evaluation/build/EvaluateApproximation -i out/ -o out/ -db MUTAG Cuneiform KarateClub
