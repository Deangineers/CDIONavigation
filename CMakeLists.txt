cmake_minimum_required(VERSION 3.28)
project(CDIONavigation)

set(CMAKE_CXX_STANDARD 20)

set(testing OFF)
include(FetchContent)

FetchContent_Declare(
        Utility
        GIT_REPOSITORY https://github.com/ThePhoneBeLinging/Utility
        GIT_TAG v1.0.0
)
FetchContent_MakeAvailable(Utility)

set_target_properties(Utility PROPERTIES POSITION_INDEPENDENT_CODE ON)

if(testing)
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG release-1.12.1  # Specify the version you need
    )
    FetchContent_MakeAvailable(googletest)
    add_subdirectory(tests)

    enable_testing()
    add_subdirectory(Simulator)

endif ()

add_subdirectory(src)