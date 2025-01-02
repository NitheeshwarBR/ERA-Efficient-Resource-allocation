#pragma once
#include "utils/data_types.hpp"
#include <random>

class MotionProcessor {
public:
    virtual MPU6050Data readMotionData();
    virtual ~MotionProcessor() = default;
};