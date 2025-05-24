cmake_minimum_required(VERSION 3.28)
project(CDIONavigation_Simulator)

set(CMAKE_CXX_STANDARD 20)


include(FetchContent)
FetchContent_Declare(
        EngineBase
        GIT_REPOSITORY https://github.com/ThePhoneBeLinging/EngineBase
        GIT_TAG v4.2.1
)

FetchContent_MakeAvailable(EngineBase)

add_executable(CDIONavigation_Simulator
    main.cpp
    )


target_link_libraries(CDIONavigation_Simulator PUBLIC CDIONavigation)
target_link_libraries(CDIONavigation_Simulator PRIVATE EngineBase)