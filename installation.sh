#!/bin/bash
#Script to install the AppOfTukeyDepth code
#prepare libs folder
unzip libs.zip
#install or-tools
git clone https://github.com/google/or-tools libs/ && make libs/or-tools/third_party && make libs/or-tools/cc && make libs/or-tools/test_cc
#install Snap-6.0
wget https://snap.stanford.edu/releases/Snap-6.0.zip libs/ && unzip libs/Snap-6.0.zip
cp libs/fl.cpp libs/Snap-6.0/glib-core/
#compile code
mkdir TukeyDepth/build && cmake && make build/
mkdir Approximation/build && cmake && make build/
mkdir Evaluation/build && cmake && make build/
mkdir out

