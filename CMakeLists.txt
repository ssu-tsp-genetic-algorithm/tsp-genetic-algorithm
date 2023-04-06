cmake_minimum_required(VERSION 3.23)
project(oriburger_genetic_tsp)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV REQUIRED)
include_directories(${/opt/homebrew/Cellar/opencv/4.7.0_2})
set(CMAKE_CXX_STANDARD 14)

include_directories(source)

add_executable(oriburger_genetic_tsp
        source/genetic.cpp
        source/genetic.h
        source/main.cpp source/kmeans.cpp source/kmeans.h)

target_link_libraries(oriburger_genetic_tsp ${OpenCV_LIBS})