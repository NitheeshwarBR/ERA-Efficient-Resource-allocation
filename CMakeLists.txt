cmake_minimum_required(VERSION 3.10)
project(RaspberryPiOptimizer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Find required packages
find_package(Threads REQUIRED)
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()

# CPR for REST client API
find_package(cpr QUIET)
if(NOT cpr_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        cpr
        GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 1.10.2
    )
    FetchContent_MakeAvailable(cpr)
endif()

# Crow for REST server API
find_package(Crow QUIET)
if(NOT Crow_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        Crow
        GIT_REPOSITORY https://github.com/crowcpp/crow
        GIT_TAG master
    )
    FetchContent_MakeAvailable(Crow)
endif()

# Explicitly define our source files
file(GLOB_RECURSE RESOURCE_SOURCES
    "${CMAKE_SOURCE_DIR}/src/resource/*.cpp"
)

file(GLOB_RECURSE GENETIC_SOURCES
    "${CMAKE_SOURCE_DIR}/src/genetic/*.cpp"
)

file(GLOB_RECURSE SENSOR_SOURCES
    "${CMAKE_SOURCE_DIR}/src/sensors/*.cpp"
)

file(GLOB_RECURSE MAIN_SOURCES
    "${CMAKE_SOURCE_DIR}/src/main.cpp"
)

# Create optimizer executable
add_executable(optimizer 
    ${MAIN_SOURCES}
    ${RESOURCE_SOURCES}
    ${GENETIC_SOURCES}
    ${SENSOR_SOURCES}
)

target_link_libraries(optimizer PRIVATE 
    Threads::Threads 
    nlohmann_json::nlohmann_json
    m  # Add math library for sin/cos functions
)

# Create API server executable
file(GLOB_RECURSE API_SOURCES 
    "${CMAKE_SOURCE_DIR}/src/api/*.cpp"
)

add_executable(api_server 
    ${API_SOURCES}
    ${RESOURCE_SOURCES}
    ${GENETIC_SOURCES}
    ${SENSOR_SOURCES}
)

target_link_libraries(api_server PRIVATE 
    Threads::Threads 
    nlohmann_json::nlohmann_json
    Crow::Crow
    m  # Add math library for sin/cos functions
)

# Create test load generators
file(GLOB_RECURSE LOAD_TEST_SOURCES 
    "${CMAKE_SOURCE_DIR}/src/load_tests/*.cpp"
)
add_executable(load_generator ${LOAD_TEST_SOURCES})
target_link_libraries(load_generator PRIVATE 
    Threads::Threads 
    cpr::cpr
    m  # Add math library for sin/cos functions
)

# Enable testing
enable_testing()

# Make sure tests directory exists before including it
if(EXISTS "${CMAKE_SOURCE_DIR}/src/tests/CMakeLists.txt")
    add_subdirectory(src/tests)
endif()

# Installation
install(TARGETS optimizer api_server load_generator
    RUNTIME DESTINATION bin
)

# Create directory for web dashboard
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/dashboard)