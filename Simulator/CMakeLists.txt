cmake_minimum_required(VERSION 3.25)
project(CDIONavigation_Simulator)

set(CMAKE_CXX_STANDARD 20)


include(FetchContent)
FetchContent_Declare(
        EngineBase
        GIT_REPOSITORY https://github.com/ThePhoneBeLinging/EngineBase
        GIT_TAG v4.2.2
)

FetchContent_MakeAvailable(EngineBase)

add_executable(CDIONavigation_Simulator
        main.cpp
        Simulator.cpp
        Simulator.h
        Objects/SimulatedRobot.cpp
        Objects/SimulatedRobot.h
        TextureLocations.h
        Objects/Ball.cpp
        Objects/Ball.h
        Objects/Cross.cpp
        Objects/Cross.h
        Objects/Egg.cpp
        Objects/Egg.h
)


target_link_libraries(CDIONavigation_Simulator PUBLIC CDIONavigation)
target_link_libraries(CDIONavigation_Simulator PRIVATE EngineBase)