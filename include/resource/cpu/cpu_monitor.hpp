#pragma once

class CPUMonitor {
public:
    virtual double getCurrentUsage();
    virtual ~CPUMonitor() = default;
};
