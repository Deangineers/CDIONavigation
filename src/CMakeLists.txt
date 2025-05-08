cmake_minimum_required(VERSION 3.28)
project(CDIONavigation)

set(CMAKE_CXX_STANDARD 20)

add_library(CDIONavigation SHARED PythonBinding.cpp
        Controllers/NavigationController.cpp
        Controllers/NavigationController.h
        Controllers/Client.cpp
        Controllers/Client.h
        Models/Command.cpp
        Models/Command.h
        Controllers/NavigationController.h
        Models/CourseObject.cpp
        Models/CourseObject.h
        Controllers/MainController.cpp
        Controllers/MainController.h
)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_link_libraries(CDIONavigation PRIVATE ws2_32)
endif()