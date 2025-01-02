#pragma once
#include "chromosome.hpp"
#include <vector>

class Population {
public:
    Population(size_t size);
    void evolve();
    Chromosome getBestChromosome() const;
    
private:
    std::vector<Chromosome> chromosomes;
};