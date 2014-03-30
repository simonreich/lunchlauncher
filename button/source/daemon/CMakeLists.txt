cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(LunchLauncherButtond)

# SET (CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "-O1 -Wall -fopenmp")        ## Optimize

add_executable (lunchlauncherbuttond main.cpp)
target_link_libraries (lunchlauncherbuttond)
