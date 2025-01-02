#include "sensors/temperature/temp_reader.hpp"

float TempReader::readTemperature() {
    return 20.0f + (std::rand() % 100) / 10.0f;
}
