#pragma once
#include "chromosome.hpp"

class Crossover {
public:
    static std::pair<Chromosome, Chromosome> crossover(
        const Chromosome& parent1, 
        const Chromosome& parent2
    );
};