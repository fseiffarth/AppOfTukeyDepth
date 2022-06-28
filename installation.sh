#!/bin/bash
#Script to install the AppOfTukeyDepth code
#prepare libs folder
unzip libs.zip
#install or-tools in libs
cd libs || exit
git clone https://github.com/google/or-tools && cd or-tools && make third_party && make cc && make test_cc && cd ..
#install Snap-6.0 in libs
wget https://snap.stanford.edu/releases/Snap-6.0.zip && unzip Snap-6.0.zip
cd ..
cp libs/fl.cpp libs/Snap-6.0/glib-core/
#compile code
mkdir TukeyDepth/build && cmake && make build/
mkdir Approximation/build && cmake && make build/
mkdir Evaluation/build && cmake && make build/
mkdir out