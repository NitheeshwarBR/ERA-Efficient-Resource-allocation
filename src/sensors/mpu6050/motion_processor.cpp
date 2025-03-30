#include "sensors/mpu6050/motion_processor.hpp"
#include <random>
#include <ctime>

MPU6050Data MotionProcessor::readMotionData() {
    // Simulated data - generate random values
    static std::mt19937 gen(static_cast<unsigned int>(std::time(nullptr)));
    std::uniform_real_distribution<float> accel_dist(-2.0f, 2.0f);
    std::uniform_real_distribution<float> gyro_dist(-250.0f, 250.0f);
    
    MPU6050Data data;
    data.accelX = accel_dist(gen);
    data.accelY = accel_dist(gen);
    data.accelZ = accel_dist(gen);
    data.gyroX = gyro_dist(gen);
    data.gyroY = gyro_dist(gen);
    data.gyroZ = gyro_dist(gen);
    
    return data;
}
