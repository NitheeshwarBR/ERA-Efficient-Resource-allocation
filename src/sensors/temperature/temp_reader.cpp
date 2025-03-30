#include "sensors/temperature/temp_reader.hpp"
#include <random>
#include <ctime>

float TempReader::readTemperature() {
    // Simple simulation - returns a random temperature between 35-65°C
    static std::mt19937 gen(static_cast<unsigned int>(std::time(nullptr)));
    std::uniform_real_distribution<float> dist(35.0f, 65.0f);
    return dist(gen);
}
