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

include_directories(../libs/libIO/include)
link_directories(../libs/libIO/include)
file(GLOB IO_LIB CONFIGURE_DEPENDS "../libs/libIO/include/*.h" "../libs/libIO/include/*.cpp")

#Evaluation of the Approximation
add_executable(EvaluateApproximation evaluation.cpp)
target_link_libraries(EvaluateApproximation ${IO_LIB})