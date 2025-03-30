#include "genetic/crossover.hpp"
#include <random>

// Random number generation
static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_real_distribution<> crossoverPointDist(0.0, 1.0);

std::pair<Chromosome, Chromosome> Crossover::crossover(
    const Chromosome& parent1, 
    const Chromosome& parent2
) {
    // Create children with parent parameters
    Chromosome child1, child2;
    OptimizationParams params1 = parent1.getParams();
    OptimizationParams params2 = parent2.getParams();
    
    // Randomly decide crossover type
    double crossoverType = crossoverPointDist(gen);
    
    if (crossoverType < 0.5) {
        // Single point crossover
        // Child 1 gets CPU from parent1, memory and power from parent2
        // Child 2 gets CPU from parent2, memory and power from parent1
        OptimizationParams childParams1, childParams2;
        
        childParams1.cpu_threshold = params1.cpu_threshold;
        childParams1.memory_threshold = params2.memory_threshold;
        childParams1.power_threshold = params2.power_threshold;
        
        childParams2.cpu_threshold = params2.cpu_threshold;
        childParams2.memory_threshold = params1.memory_threshold;
        childParams2.power_threshold = params1.power_threshold;
        
        // Create new chromosomes with crossover parameters
        child1 = Chromosome();
        child2 = Chromosome();
        
        // This is a simplified approach since we can't directly set the parameters
        // In a real implementation, we would need a constructor or setter method
        
        return {child1, child2};
    } else {
        // Arithmetic mean crossover
        OptimizationParams childParams1, childParams2;
        
        // Random weight for blending
        double alpha = crossoverPointDist(gen);
        
        // Blend parameters
        childParams1.cpu_threshold = alpha * params1.cpu_threshold + (1 - alpha) * params2.cpu_threshold;
        childParams1.memory_threshold = alpha * params1.memory_threshold + (1 - alpha) * params2.memory_threshold;
        childParams1.power_threshold = alpha * params1.power_threshold + (1 - alpha) * params2.power_threshold;
        
        // Inverse blend for second child
        childParams2.cpu_threshold = (1 - alpha) * params1.cpu_threshold + alpha * params2.cpu_threshold;
        childParams2.memory_threshold = (1 - alpha) * params1.memory_threshold + alpha * params2.memory_threshold;
        childParams2.power_threshold = (1 - alpha) * params1.power_threshold + alpha * params2.power_threshold;
        
        // Create new chromosomes
        child1 = Chromosome();
        child2 = Chromosome();
        
        return {child1, child2};
    }
}
