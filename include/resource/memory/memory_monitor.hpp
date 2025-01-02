#pragma once

class MemoryMonitor {
public:
    virtual double getCurrentUsage();
    virtual ~MemoryMonitor() = default;
};