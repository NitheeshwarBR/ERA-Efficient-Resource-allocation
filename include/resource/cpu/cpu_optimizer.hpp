#pragma once

class CPUOptimizer {
public:
    virtual void optimize(double threshold);
    virtual ~CPUOptimizer() = default;
};

