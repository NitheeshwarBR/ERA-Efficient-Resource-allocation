#pragma once
#include "chromosome.hpp"
#include "utils/data_types.hpp"

class FitnessEvaluator {
public:
    static double evaluate(
        const Chromosome& chromosome, 
        const SystemResources& resources
    );
};