cmake_minimum_required(VERSION 3.28)
project(CDIONavigation)

set(CMAKE_CXX_STANDARD 20)

add_library(CDIONavigation SHARED PythonBinding.cpp
        Controllers/NavigationController.cpp
        Controllers/NavigationController.h
        Models/Command.cpp
        Models/Command.h
        Controllers/NavigationController.h
        Models/CourseObject.cpp
        Models/CourseObject.h
        Controllers/MainController.cpp
        Controllers/MainController.h
        Controllers/Clients/IClient.h
        Controllers/Clients/LinuxClient.cpp
        Controllers/Clients/LinuxClient.h
        Controllers/Clients/MockClient.cpp
        Controllers/Clients/MockClient.h
)

target_link_libraries(CDIONavigation PUBLIC Utility)