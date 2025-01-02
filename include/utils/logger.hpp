#pragma once
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>

class Logger {
public:
    static void log(const std::string& filename, const std::string& message) {
        std::ofstream file(filename, std::ios::app);
        auto now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);
        file << std::ctime(&time) << message << std::endl;
    }
};
