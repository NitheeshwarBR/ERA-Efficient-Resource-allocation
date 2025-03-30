# Integration tests CMakeLists.txt

# Explicitly include the necessary resource monitor implementation files
add_executable(system_tests 
    system_test.cpp
    ${CMAKE_SOURCE_DIR}/src/resource/cpu/cpu_monitor.cpp
    ${CMAKE_SOURCE_DIR}/src/resource/memory/memory_monitor.cpp
    ${CMAKE_SOURCE_DIR}/src/resource/power/power_monitor.cpp
    ${CMAKE_SOURCE_DIR}/src/genetic/chromosome.cpp
    ${CMAKE_SOURCE_DIR}/src/genetic/population.cpp
    ${CMAKE_SOURCE_DIR}/src/genetic/crossover.cpp
    ${CMAKE_SOURCE_DIR}/src/genetic/mutation.cpp
    ${CMAKE_SOURCE_DIR}/src/genetic/fitness.cpp
)

# Link with libraries and dependencies
target_link_libraries(system_tests
    PRIVATE
    GTest::gtest
    GTest::gtest_main
    Threads::Threads
    m  # Link with math library for sin/cos
)

# Add system test to CTest
add_test(NAME SystemTests COMMAND system_tests)
