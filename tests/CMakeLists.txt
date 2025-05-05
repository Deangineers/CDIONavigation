cmake_minimum_required(VERSION 3.25)
project(CDIONavigation_Tests)

set(CMAKE_CXX_STANDARD 20)

set(SOURCES
        NavigationControllerTests.cpp
        NavigationControllerTests.h
)
add_executable(CDIONavigation_Tests ${SOURCES})

target_link_libraries(CDIONavigation_Tests PUBLIC
        GTest::gtest
        GTest::gtest_main
        CDIONavigation
)