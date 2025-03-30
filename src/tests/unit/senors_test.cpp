#include <gtest/gtest.h>
#include "sensors/temperature/temp_reader.hpp"
#include "sensors/mpu6050/motion_processor.hpp"
#include "sensors/camera/camera_capture.hpp"

// Test fixture for sensor tests
class SensorsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code that will be called before each test
    }

    void TearDown() override {
        // Cleanup code that will be called after each test
    }
};

// Test temperature sensor
TEST_F(SensorsTest, TemperatureSensorWorks) {
    TempReader sensor;
    float temp = sensor.readTemperature();
    
    // Temperature should be in a reasonable range for computing devices
    EXPECT_GE(temp, 10.0f);  // At least 10°C
    EXPECT_LE(temp, 90.0f);  // At most 90°C (very hot but not impossible)
}

// Test motion sensor
TEST_F(SensorsTest, MotionSensorWorks) {
    MotionProcessor sensor;
    MPU6050Data data = sensor.readMotionData();
    
    // Check that acceleration data is reasonable
    // Earth's gravity is approximately 9.8 m/s²
    EXPECT_LE(std::abs(data.accelX), 20.0f);
    EXPECT_LE(std::abs(data.accelY), 20.0f);
    EXPECT_LE(std::abs(data.accelZ), 20.0f);
    
    // Check that gyroscope data is reasonable
    EXPECT_LE(std::abs(data.gyroX), 500.0f);
    EXPECT_LE(std::abs(data.gyroY), 500.0f);
    EXPECT_LE(std::abs(data.gyroZ), 500.0f);
}

// Test camera
TEST_F(SensorsTest, CameraWorks) {
    CameraCapture camera;
    CameraFrame frame = camera.captureFrame();
    
    // Check that we got a valid frame
    EXPECT_GT(frame.width, 0);
    EXPECT_GT(frame.height, 0);
    EXPECT_GT(frame.size, 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
