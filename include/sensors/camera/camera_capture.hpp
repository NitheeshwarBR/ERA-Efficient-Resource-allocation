#pragma once
#include "utils/data_types.hpp"

class CameraCapture {
public:
    virtual CameraFrame captureFrame();
    virtual ~CameraCapture() = default;
};