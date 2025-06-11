cmake_minimum_required(VERSION 3.25)
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
        Controllers/MathUtil.cpp
        Controllers/MathUtil.h
        Controllers/ClientController.cpp
        Controllers/ClientController.h
        Models/Vector.cpp
        Models/Vector.h
)

target_link_libraries(CDIONavigation PUBLIC Utility)