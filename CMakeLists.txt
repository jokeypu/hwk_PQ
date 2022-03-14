cmake_minimum_required(VERSION 3.10) 

# set the project name 
project(test_pro) 

set(CMAKE_CXX_COMPILER "g++")

set( CMAKE_BUILD_TYPE Debug )
#set( CMAKE_BUILD_TYPE Release )

set(CMAKE_CXX_STANDARD 11) 
set(CMAKE_CXX_STANDARD_REQUIRED True) 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -march=native -O3 -pthread" )

# add the executable 
add_executable(test_pro test.cxx EventFilter.cxx) 
target_include_directories(test_pro PUBLIC 
                            "${PROJECT_SOURCE_DIR}"
                            )
