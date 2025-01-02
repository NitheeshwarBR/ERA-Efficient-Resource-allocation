cmake_minimum_required(VERSION 3.10)
project(RaspberryPiOptimizer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add source files
file(GLOB_RECURSE SOURCES 
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

# Create executable
add_executable(optimizer ${SOURCES})

# Link against pthread
find_package(Threads REQUIRED)
target_link_libraries(optimizer PRIVATE Threads::Threads)