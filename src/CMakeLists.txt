cmake_minimum_required(VERSION 3.25)
project(CDIONavigationLib)

set(CMAKE_CXX_STANDARD 23)

find_package(OpenCV REQUIRED)

add_library(CDIONavigationLib STATIC
        PythonBinding.cpp
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
        Models/VectorWithStartPos.cpp
        Models/VectorWithStartPos.h
        Models/Egg.cpp
        Models/Egg.h
        Controllers/ImageProcessing/ImageProcessor.cpp
        Controllers/ImageProcessing/ImageProcessor.h
        Controllers/ImageProcessing/CloudyImageProcessor.cpp
        Controllers/ImageProcessing/CloudyImageProcessor.h
        Controllers/ImageProcessing/BallProcessor.cpp
        Controllers/ImageProcessing/BallProcessor.h
        Controllers/ImageProcessing/WallProcessor.cpp
        Controllers/ImageProcessing/WallProcessor.h
        Controllers/ObjectCounter.cpp
        Controllers/ObjectCounter.h
)

target_link_libraries(CDIONavigationLib PUBLIC Utility)
target_include_directories(CDIONavigationLib PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(CDIONavigationLib PRIVATE ${OpenCV_LIBS})