#include "resource/memory/memory_monitor.hpp"
#include <fstream>
#include <string>
#include <cstdlib>
#include <sstream> // Add sstream if needed

double MemoryMonitor::getCurrentUsage() {
    #ifdef __linux__
    // Linux implementation - read from /proc/meminfo
    std::ifstream meminfo("/proc/meminfo");
    std::string line;
    
    long total_memory = 0;
    long free_memory = 0;
    long buffers = 0;
    long cached = 0;
    
    while (std::getline(meminfo, line)) {
        if (line.find("MemTotal:") != std::string::npos) {
            sscanf(line.c_str(), "MemTotal: %ld", &total_memory);
        } else if (line.find("MemFree:") != std::string::npos) {
            sscanf(line.c_str(), "MemFree: %ld", &free_memory);
        } else if (line.find("Buffers:") != std::string::npos) {
            sscanf(line.c_str(), "Buffers: %ld", &buffers);
        } else if (line.find("Cached:") != std::string::npos && 
                  line.find("SwapCached:") == std::string::npos) {
            sscanf(line.c_str(), "Cached: %ld", &cached);
        }
    }
    
    // Close the file
    meminfo.close();
    
    // Calculate used memory
    long used_memory = total_memory - free_memory - buffers - cached;
    
    // Calculate memory usage percentage
    double memory_percentage = 100.0 * (double)used_memory / (double)total_memory;
    
    return memory_percentage;
    #else
    // Mock implementation for non-Linux platforms
    static double lastUsage = 40.0;
    
    // Simulate fluctuating memory usage
    double delta = (rand() % 100) / 20.0 - 2.5;
    lastUsage += delta;
    
    // Keep within reasonable bounds
    if (lastUsage < 10.0) lastUsage = 10.0;
    if (lastUsage > 90.0) lastUsage = 90.0;
    
    return lastUsage;
    #endif
}
