#pragma once

class PowerMonitor {
public:
    virtual double getCurrentUsage();
    virtual ~PowerMonitor() = default;
};
