cmake_minimum_required(VERSION 3.23)
project(oriburger_genetic_tsp)

set(CMAKE_CXX_STANDARD 14)

include_directories(source)

add_executable(oriburger_genetic_tsp
        source/genetic.cpp
        source/genetic.h
        source/main.cpp source/treeRouteFinder.h source/treeRouteFinder.cpp)
