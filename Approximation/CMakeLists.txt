cmake_minimum_required(VERSION 3.20)
project(Approximation)

set(CMAKE_CXX_STANDARD 17)


set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS "-std=c++17")

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

include_directories(../libs/Snap-6.0/snap-core)
link_directories(../libs/Snap-6.0/snap-core)
include_directories(../libs/Snap-6.0/glib-core)
link_directories(../libs/Snap-6.0/glib-core)

include_directories(../libs/libIO/include)
link_directories(../libs/libIO/include)
file(GLOB IO_LIB CONFIGURE_DEPENDS "../libs/libIO/include/*.h" "../libs/libIO/include/*.cpp")



#Approximate Tukey Depth
add_executable(ApproximateTukeyDepth src/approximate_tukey.cpp
        ../libs/Snap-6.0/snap-core/Snap.cpp
        Data/GraphData.h Data/GraphData.cpp
        Data/OuterplanarGraphData.h Data/OuterplanarGraphData.cpp
        Utils/StaticFunctions.h Utils/StaticFunctions.cpp
        Utils/GraphFunctions.h Utils/GraphFunctions.cpp Utils/GraphFunctions.txx
        Utils/OuterplanarSubgraph.h Utils/OuterplanarSubgraph.cpp Utils/OuterplanarSubgraphDFS.h Utils/OuterplanarSubgraphDFS.cpp
        Utils/graphio.h Utils/graphio.cpp
        Utils/Graphs.h Utils/Graphs.cpp
        Utils/LoadTUDortmundGraphData.cpp Utils/LoadTUDortmundGraphData.h
        ClosureOperators/BaseOperator.cpp ClosureOperators/BaseOperator.h ClosureOperators/GraphClosures.cpp ClosureOperators/GraphClosures.h
        InitUpdate/InitUpdateBase.h InitUpdate/InitUpdateBase.cpp InitUpdate/GraphInitUpdate.h InitUpdate/GraphInitUpdate.cpp
        Algorithms/TukeyDepth.h Algorithms/TukeyDepth.txx )

#Computation of the core
add_executable(ComputeCore src/cores.cpp
        ../libs/Snap-6.0/snap-core/Snap.cpp
        Data/GraphData.h Data/GraphData.cpp
        Data/OuterplanarGraphData.h Data/OuterplanarGraphData.cpp
        Utils/StaticFunctions.h Utils/StaticFunctions.cpp
        Utils/GraphFunctions.h Utils/GraphFunctions.cpp Utils/GraphFunctions.txx
        Utils/OuterplanarSubgraph.h Utils/OuterplanarSubgraph.cpp Utils/OuterplanarSubgraphDFS.h Utils/OuterplanarSubgraphDFS.cpp
        Utils/graphio.h Utils/graphio.cpp
        Utils/Graphs.h Utils/Graphs.cpp
        Utils/LoadTUDortmundGraphData.cpp Utils/LoadTUDortmundGraphData.h
        ClosureOperators/BaseOperator.cpp ClosureOperators/BaseOperator.h ClosureOperators/GraphClosures.cpp ClosureOperators/GraphClosures.h
        InitUpdate/InitUpdateBase.h InitUpdate/InitUpdateBase.cpp InitUpdate/GraphInitUpdate.h InitUpdate/GraphInitUpdate.cpp
        Algorithms/TukeyDepth.h Algorithms/TukeyDepth.txx )

target_link_libraries(ApproximateTukeyDepth ${IO_LIB})
target_link_libraries(ComputeCore ${IO_LIB})