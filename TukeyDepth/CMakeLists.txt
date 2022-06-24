cmake_minimum_required(VERSION 3.20)
project(TukeyDepth)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

set(CMAKE_PREFIX_PATH "../../LibsExternal/or-tools")
find_package(ortools CONFIG REQUIRED)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

include_directories(../libs/libGraph/include)
link_directories(../libs/libGraph/include)
file(GLOB GRAPH_LIB CONFIGURE_DEPENDS "../libs/libGraph/include/*.h" "../libs/libGraph/include/*.cpp")

include_directories(../libs/libIO/include)
link_directories(../libs/libIO/include)
file(GLOB IO_LIB CONFIGURE_DEPENDS "../libs/libIO/include/*.h" "../libs/libIO/include/*.cpp" "../libs/libIO/include/*.txx")

add_executable(TukeyDepth main.cpp Algorithm/TukeyDepth.cpp Algorithm/TukeyDepth.h ${GRAPH_LIB} ${IO_LIB})
target_link_libraries(TukeyDepth ortools::ortools)
