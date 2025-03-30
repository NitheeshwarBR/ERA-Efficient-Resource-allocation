# Tests CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

# Find GTest package
find_package(GTest QUIET)
if(NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
    )
    FetchContent_MakeAvailable(googletest)
endif()

# Add subdirectories
add_subdirectory(unit)
add_subdirectory(integration)
