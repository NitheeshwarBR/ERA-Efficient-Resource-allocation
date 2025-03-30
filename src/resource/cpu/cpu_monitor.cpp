#include "resource/cpu/cpu_monitor.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <sstream> // Added missing include for istringstream
#include <cstdlib> // For rand() function

double CPUMonitor::getCurrentUsage() {
    #ifdef __linux__
    // Linux implementation - read from /proc/stat
    std::ifstream stat_file("/proc/stat");
    std::string line;
    std::getline(stat_file, line);
    
    // Parse CPU statistics
    std::vector<long> cpu_times;
    std::string token;
    std::istringstream iss(line);
    
    // Skip "cpu" label
    iss >> token;
    
    // Read CPU times
    while (iss >> token) {
        cpu_times.push_back(std::stol(token));
    }
    
    // Close the file
    stat_file.close();
    
    // Calculate total and idle times
    long user = cpu_times[0];
    long nice = cpu_times[1];
    long system = cpu_times[2];
    long idle = cpu_times[3];
    long iowait = cpu_times[4];
    long irq = cpu_times[5];
    long softirq = cpu_times[6];
    long steal = cpu_times[7];
    
    long total_idle = idle + iowait;
    long total = user + nice + system + idle + iowait + irq + softirq + steal;
    
    // Take second measurement after a short delay
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::ifstream stat_file2("/proc/stat");
    std::getline(stat_file2, line);
    
    // Parse CPU statistics again
    std::vector<long> cpu_times2;
    std::istringstream iss2(line);
    
    // Skip "cpu" label
    iss2 >> token;
    
    // Read CPU times
    while (iss2 >> token) {
        cpu_times2.push_back(std::stol(token));
    }
    
    // Close the file
    stat_file2.close();
    
    // Calculate total and idle times for second measurement
    long user2 = cpu_times2[0];
    long nice2 = cpu_times2[1];
    long system2 = cpu_times2[2];
    long idle2 = cpu_times2[3];
    long iowait2 = cpu_times2[4];
    long irq2 = cpu_times2[5];
    long softirq2 = cpu_times2[6];
    long steal2 = cpu_times2[7];
    
    long total_idle2 = idle2 + iowait2;
    long total2 = user2 + nice2 + system2 + idle2 + iowait2 + irq2 + softirq2 + steal2;
    
    // Calculate the difference
    double total_delta = total2 - total;
    double idle_delta = total_idle2 - total_idle;
    
    // Calculate CPU percentage
    double cpu_percentage = 100.0 * (1.0 - idle_delta / total_delta);
    
    return cpu_percentage;
    #else
    // Mock implementation for non-Linux platforms
    static double lastUsage = 50.0;
    
    // Simulate fluctuating CPU usage
    double delta = (rand() % 100) / 10.0 - 5.0;
    lastUsage += delta;
    
    // Keep within reasonable bounds
    if (lastUsage < 5.0) lastUsage = 5.0;
    if (lastUsage > 95.0) lastUsage = 95.0;
    
    return lastUsage;
    #endif
}
