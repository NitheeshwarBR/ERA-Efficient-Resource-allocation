#include "genetic/population.hpp"
#include "genetic/crossover.hpp"
#include "genetic/mutation.hpp"
#include "resource/cpu/cpu_monitor.hpp"
#include "resource/memory/memory_monitor.hpp"
#include "resource/power/power_monitor.hpp"
#include <algorithm>
#include <random>
#include <memory>

// Constants for genetic algorithm
constexpr double MUTATION_RATE = 0.1;
constexpr double ELITE_PERCENTAGE = 0.2;

// Random engine for selection
static std::random_device rd;
static std::mt19937 gen(rd());

Population::Population(size_t size) : chromosomes(size) {
    // Initialize with random chromosomes
    for (auto& chromosome : chromosomes) {
        chromosome = Chromosome();
    }
}

void Population::evolve() {
    // Get current resource usage to evaluate fitness
    CPUMonitor cpuMonitor;
    MemoryMonitor memMonitor;
    PowerMonitor powerMonitor;
    
    SystemResources currentResources = {
        cpuMonitor.getCurrentUsage(),
        memMonitor.getCurrentUsage(),
        powerMonitor.getCurrentUsage()
    };
    
    // Calculate fitness for each chromosome
    for (auto& chromosome : chromosomes) {
        chromosome.calculateFitness(currentResources);
    }
    
    // Sort by fitness (descending)
    std::sort(chromosomes.begin(), chromosomes.end(), 
        [](const Chromosome& a, const Chromosome& b) {
            return a.getFitness() > b.getFitness();
        }
    );
    
    // Number of elites to keep
    size_t eliteCount = static_cast<size_t>(chromosomes.size() * ELITE_PERCENTAGE);
    if (eliteCount == 0) eliteCount = 1;
    
    // Create new population
    std::vector<Chromosome> newPopulation;
    
    // Add elites directly
    for (size_t i = 0; i < eliteCount; ++i) {
        newPopulation.push_back(chromosomes[i]);
    }
    
    // Create selection probability distribution based on fitness
    std::vector<double> fitnesses;
    for (const auto& chromosome : chromosomes) {
        fitnesses.push_back(chromosome.getFitness());
    }
    std::discrete_distribution<size_t> selection(fitnesses.begin(), fitnesses.end());
    
    // Fill the rest of the population with crossover and mutation
    while (newPopulation.size() < chromosomes.size()) {
        // Select parents
        size_t parent1Idx = selection(gen);
        size_t parent2Idx = selection(gen);
        
        // Ensure different parents
        while (parent2Idx == parent1Idx && chromosomes.size() > 1) {
            parent2Idx = selection(gen);
        }
        
        // Perform crossover
        auto [child1, child2] = Crossover::crossover(
            chromosomes[parent1Idx], 
            chromosomes[parent2Idx]
        );
        
        // Perform mutation
        Mutation::mutate(child1, MUTATION_RATE);
        Mutation::mutate(child2, MUTATION_RATE);
        
        // Add to new population
        newPopulation.push_back(child1);
        if (newPopulation.size() < chromosomes.size()) {
            newPopulation.push_back(child2);
        }
    }
    
    // Replace old population
    chromosomes = std::move(newPopulation);
}

Chromosome Population::getBestChromosome() const {
    // Find chromosome with highest fitness
    auto bestIt = std::max_element(chromosomes.begin(), chromosomes.end(),
        [](const Chromosome& a, const Chromosome& b) {
            return a.getFitness() < b.getFitness();
        }
    );
    
    return (bestIt != chromosomes.end()) ? *bestIt : Chromosome();
}
