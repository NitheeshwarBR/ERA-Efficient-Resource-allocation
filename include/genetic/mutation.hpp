#pragma once
#include "chromosome.hpp"

class Mutation {
public:
    static void mutate(Chromosome& chromosome, double mutationRate);
};