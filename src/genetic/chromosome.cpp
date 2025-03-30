#include "genetic/chromosome.hpp"
#include <random>
#include <algorithm>

// Random number generation
static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_real_distribution<> cpuThresholdDist(30.0, 90.0);    // CPU threshold 30-90%
static std::uniform_real_distribution<> memThresholdDist(40.0, 95.0);    // Memory threshold 40-95%
static std::uniform_real_distribution<> powerThresholdDist(2.0, 10.0);   // Power threshold 2-10W

Chromosome::Chromosome() : fitness(0.0) {
    // Initialize with random parameters
    params.cpu_threshold = cpuThresholdDist(gen);
    params.memory_threshold = memThresholdDist(gen);
    params.power_threshold = powerThresholdDist(gen);
}

void Chromosome::calculateFitness(const SystemResources& current) {
    // Fitness calculation based on how well the thresholds match current usage
    // Higher fitness means better match to current resources
    
    // Calculate distance from current usage to threshold
    double cpuDistance = std::abs(current.cpu_usage - params.cpu_threshold);
    double memDistance = std::abs(current.memory_usage - params.memory_threshold);
    double powerDistance = std::abs(current.power_usage - params.power_threshold);
    
    // Normalize distances (lower is better)
    double normalizedCpuDistance = 1.0 - (cpuDistance / 100.0);
    double normalizedMemDistance = 1.0 - (memDistance / 100.0);
    double normalizedPowerDistance = 1.0 - (powerDistance / 10.0);
    
    // Calculate combined fitness (weighted average)
    fitness = (0.4 * normalizedCpuDistance) + 
              (0.4 * normalizedMemDistance) + 
              (0.2 * normalizedPowerDistance);
    
    // Apply penalty if thresholds are too low (causes underutilization)
    if (params.cpu_threshold < current.cpu_usage * 0.7) {
        fitness *= 0.8;
    }
    if (params.memory_threshold < current.memory_usage * 0.7) {
        fitness *= 0.8;
    }
    if (params.power_threshold < current.power_usage * 0.7) {
        fitness *= 0.8;
    }
    
    // Apply penalty if thresholds are too high (causes resource exhaustion)
    if (params.cpu_threshold > current.cpu_usage * 1.3) {
        fitness *= 0.9;
    }
    if (params.memory_threshold > current.memory_usage * 1.3) {
        fitness *= 0.9;
    }
    if (params.power_threshold > current.power_usage * 1.3) {
        fitness *= 0.9;
    }
}

OptimizationParams Chromosome::getParams() const {
    return params;
}

double Chromosome::getFitness() const {
    return fitness;
}
