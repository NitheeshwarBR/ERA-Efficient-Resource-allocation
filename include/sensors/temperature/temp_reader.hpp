#pragma once
#include <random>

class TempReader {
public:
    virtual float readTemperature();
    virtual ~TempReader() = default;
};