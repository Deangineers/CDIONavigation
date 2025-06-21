cmake_minimum_required(VERSION 3.25)
project(CDIONavigation_Tests)

set(CMAKE_CXX_STANDARD 23)
find_package(OpenCV REQUIRED)

file(GLOB_RECURSE SRC_FILES ../src/*.cpp)
set(SOURCES
        BallDectectionTests.cpp
        FrontBackDetectionTests.cpp
        ${SRC_FILES}
        TestBallProcessor.cpp
        TestBallProcessor.h
)
add_executable(CDIONavigation_Tests ${SOURCES})

target_link_libraries(CDIONavigation_Tests PUBLIC
        GTest::gtest
        GTest::gtest_main
        Utility
        CDIONavigationLib
)
target_include_directories(CDIONavigation_Tests PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(CDIONavigation_Tests PRIVATE ${OpenCV_LIBS})