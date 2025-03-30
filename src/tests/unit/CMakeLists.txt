# Unit tests CMakeLists.txt

# Add sensor tests executable with implementations
add_executable(sensor_tests 
    senors_test.cpp
    ${CMAKE_SOURCE_DIR}/src/sensors/temperature/temp_reader.cpp
    ${CMAKE_SOURCE_DIR}/src/sensors/mpu6050/motion_processor.cpp
    ${CMAKE_SOURCE_DIR}/src/sensors/camera/camera_capture.cpp
)

# Link with libraries
target_link_libraries(sensor_tests
    PRIVATE
    GTest::gtest
    GTest::gtest_main
    Threads::Threads
)

# Add sensor test to CTest
add_test(NAME SensorTests COMMAND sensor_tests)
