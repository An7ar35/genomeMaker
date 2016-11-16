cmake_minimum_required(VERSION 3.5)
project(genomeMaker)

#// Project flags
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -std=c++14 -pthread")
#// Setting the build output to ./build
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/builds/")

#// GTest framework for unit testing
enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

#// NCurses library
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

set(SOURCE_FILES
        src/tools/Randomiser.cpp
        src/tools/Randomiser.h
        src/tools/GenomeCreator.cpp
        src/tools/GenomeCreator.h
        src/gmaker.cpp
        src/tools/SequencerSim.cpp
        src/tools/SequencerSim.h
        src/containers/FileOptions.h
        src/cli/cli.h
        src/cli/cli.cpp src/containers/Buffers.h)

set(TEST_FILES
        tests/Creator_tests.cpp
        tests/main.cpp
        tests/MOCK_Randomiser.h
        tests/MOCK_Writer.h
        tests/MOCK_Reader.h
        tests/SequencerSim_Tests.h)

include_directories(include)

add_executable(
        genomeMaker
        ${SOURCE_FILES})

target_link_libraries(genomeMaker ${CURSES_LIBRARIES})

add_executable(
        genomeMaker_Tests
        ${TEST_FILES})

target_link_libraries(genomeMaker_Tests gtest ${CURSES_LIBRARIES})

add_test(genomeMaker_Tests ${TEST_FILES})