cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(LunchLauncherButtond)

# SET (CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "-O1 -Wall -fopenmp")        ## Optimize

include_directories(${QT_INCLUDES} ${QT_LIBRARY_DIRS} ${CMAKE_CURRENT_BINARY_DIR})

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt4 REQUIRED)
find_package(Qt4 COMPONENTS QtWebKit REQUIRED)
include( ${QT_USE_FILE} )


set(lunchlauncherbuttond_SOURCES main.cpp)
set(lunchlauncherbuttond_HEADERS)

add_definitions(${QT_DEFINITIONS})

add_executable (lunchlauncherbuttond ${lunchlauncherbuttond_SOURCES} ${lunchlauncherbuttond_HEADERS_MOC})
target_link_libraries (lunchlauncherbuttond ${QT_QTCORE_LIBRARY} ${QT_QTWEBVIEW_LIBRARY} ${QT_LIBRARIES})
