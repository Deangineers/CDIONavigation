cmake_minimum_required(VERSION 3.25)
project(CDIONavigation)

set(CMAKE_CXX_STANDARD 23)

set(testing OFF)
include(FetchContent)
find_package(OpenCV REQUIRED)
FetchContent_Declare(
        Utility
        GIT_REPOSITORY https://github.com/ThePhoneBeLinging/Utility
        GIT_TAG v1.0.0
)
FetchContent_MakeAvailable(Utility)

set_target_properties(Utility PROPERTIES POSITION_INDEPENDENT_CODE ON)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1  # Specify the version you need
)

add_executable(CDIONavigation
        main.cpp
)

FetchContent_MakeAvailable(googletest)
add_subdirectory(src)
add_subdirectory(tests)

enable_testing()

target_link_libraries(CDIONavigation PUBLIC CDIONavigationLib)
target_include_directories(CDIONavigation PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(CDIONavigation PRIVATE ${OpenCV_LIBS})
