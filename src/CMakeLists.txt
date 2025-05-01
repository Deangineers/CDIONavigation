cmake_minimum_required(VERSION 3.28)
project(CDIONavigation)

set(CMAKE_CXX_STANDARD 20)

add_library(CDIONavigation SHARED PythonBinding.cpp
        NavigationController.cpp
        NavigationController.h
        CourseObject.cpp
        CourseObject.h
        MainController.cpp
        MainController.h
)
