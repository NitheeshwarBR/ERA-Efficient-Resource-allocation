#pragma once
#include "utils/data_types.hpp"
#include <vector>

struct OptimizationParams {
    double cpu_threshold;
    double memory_threshold;
    double power_threshold;
};

class Chromosome {
public:
    Chromosome();
    void calculateFitness(const SystemResources& current);
    OptimizationParams getParams() const;
    double getFitness() const;
    
private:
    OptimizationParams params;
    double fitness;
};