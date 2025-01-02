#pragma once
#include <array>
#include <stddef.h>

struct MPU6050Data {
    float accelX, accelY, accelZ;
    float gyroX, gyroY, gyroZ;
};

struct CameraFrame {
    int width{};
    int height{};
    size_t size{};
};


struct SystemResources {
    double cpu_usage;
    double memory_usage;
    double power_usage;
};

