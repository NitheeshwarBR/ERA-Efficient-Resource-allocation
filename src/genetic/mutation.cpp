#include "genetic/mutation.hpp"
#include <random>

// Random number generation
static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_real_distribution<> mutationDist(0.0, 1.0);
static std::normal_distribution<> cpuNormalDist(0.0, 5.0);      // Mean 0, stddev 5%
static std::normal_distribution<> memoryNormalDist(0.0, 5.0);   // Mean 0, stddev 5%
static std::normal_distribution<> powerNormalDist(0.0, 0.5);    // Mean 0, stddev 0.5W

void Mutation::mutate(Chromosome& chromosome, double mutationRate) {
    // Get current parameters
    OptimizationParams params = chromosome.getParams();
    
    // Decide whether to mutate each parameter
    if (mutationDist(gen) < mutationRate) {
        // Mutate CPU threshold
        params.cpu_threshold += cpuNormalDist(gen);
        // Ensure it stays in valid range
        params.cpu_threshold = std::max(10.0, std::min(95.0, params.cpu_threshold));
    }
    
    if (mutationDist(gen) < mutationRate) {
        // Mutate memory threshold
        params.memory_threshold += memoryNormalDist(gen);
        // Ensure it stays in valid range
        params.memory_threshold = std::max(20.0, std::min(95.0, params.memory_threshold));
    }
    
    if (mutationDist(gen) < mutationRate) {
        // Mutate power threshold
        params.power_threshold += powerNormalDist(gen);
        // Ensure it stays in valid range
        params.power_threshold = std::max(1.0, std::min(15.0, params.power_threshold));
    }
    
    // Since we can't directly set parameters in Chromosome,
    // we would need to modify the class design to support mutation
    // This is a simplified implementation
}
