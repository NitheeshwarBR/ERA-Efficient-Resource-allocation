#pragma once

class MemoryOptimizer {
public:
    virtual void optimize(double threshold);
    virtual ~MemoryOptimizer() = default;
};