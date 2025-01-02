#include "sensors/mpu6050/motion_processor.hpp"

MPU6050Data MotionProcessor::readMotionData() {
    return {
        (float)(std::rand() % 2000 - 1000) / 100.0f,
        (float)(std::rand() % 2000 - 1000) / 100.0f,
        (float)(std::rand() % 2000 - 1000) / 100.0f,
        (float)(std::rand() % 360),
        (float)(std::rand() % 360),
        (float)(std::rand() % 360)
    };
}
