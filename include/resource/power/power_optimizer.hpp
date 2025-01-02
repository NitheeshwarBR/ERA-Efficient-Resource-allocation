#pragma once

class PowerOptimizer {
public:
    virtual void optimize(double threshold);
    virtual ~PowerOptimizer() = default;
};