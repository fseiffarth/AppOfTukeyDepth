#!/bin/bash
#Script to install the AppOfTukeyDepth code
#prepare libs folder
#unzip libs.zip && cd libs
#install or-tools
#git clone https://github.com/google/or-tools && cd or-tools && make third_party && make cc && make test_cc
#install Snap-6.0
cd .. && wget https://snap.stanford.edu/releases/Snap-6.0.zip && unzip Snap-6.0.zip
cp fl.cpp Snap-6.0/glib-core/
#compile code
cd .. && cd TukeyDepth && mkdir build && cd build && cmake .. && make 
cd .. && cd .. && cd Approximation && mkdir build && cd build && cmake .. && make 
cd .. && cd .. && cd Evaluation && mkdir build && cd build && cmake .. && make 
cd .. && cd ..
mkdir out

