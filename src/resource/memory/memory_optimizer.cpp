#include "resource/memory/memory_optimizer.hpp"
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cstdlib> // Required for system() function
#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <chrono>
#include <thread>

void MemoryOptimizer::optimize(double threshold) {
    #ifdef __linux__
    // Get current memory usage
    std::ifstream meminfo("/proc/meminfo");
    if (!meminfo.is_open()) {
        return;
    }
    
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
    meminfo.close();
    
    // Calculate used memory
    long used_memory = total_memory - free_memory - buffers - cached;
    double memory_percentage = 100.0 * (double)used_memory / (double)total_memory;
    
    // If memory usage is above threshold, try to optimize
    if (memory_percentage > threshold) {
        // Step 1: Try to drop caches
        if (memory_percentage > threshold + 10.0) {
            // Write to /proc/sys/vm/drop_caches to free up cache
            // This requires root privileges
            std::system("sync"); // Use std::system instead of system
            std::ofstream dropCaches("/proc/sys/vm/drop_caches");
            if (dropCaches.is_open()) {
                dropCaches << "3" << std::endl;
                dropCaches.close();
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        }
        
        // Step 2: Try to compact memory
        if (memory_percentage > threshold + 5.0) {
            std::ofstream compactMemory("/proc/sys/vm/compact_memory");
            if (compactMemory.is_open()) {
                compactMemory << "1" << std::endl;
                compactMemory.close();
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        }
        
        // Step 3: If critical, find and kill memory-hogging processes
        if (memory_percentage > threshold + 15.0) {
            // Open /proc directory to scan processes
            DIR* procDir = opendir("/proc");
            if (procDir) {
                struct dirent* entry;
                std::vector<std::pair<int, long>> processList;
                
                // Scan all processes
                while ((entry = readdir(procDir)) != nullptr) {
                    // Check if entry is a directory and is a number (PID)
                    if (entry->d_type == DT_DIR) {
                        int pid = 0;
                        try {
                            pid = std::stoi(entry->d_name);
                        } catch (...) {
                            continue; // Not a number, skip
                        }
                        
                        // Read process memory info
                        std::stringstream statusPath;
                        statusPath << "/proc/" << pid << "/status";
                        std::ifstream statusFile(statusPath.str());
                        
                        if (statusFile.is_open()) {
                            std::string statusLine;
                            long rss = 0;
                            
                            while (std::getline(statusFile, statusLine)) {
                                if (statusLine.find("VmRSS:") != std::string::npos) {
                                    sscanf(statusLine.c_str(), "VmRSS: %ld", &rss);
                                    break;
                                }
                            }
                            statusFile.close();
                            
                            if (rss > 0) {
                                processList.push_back({pid, rss});
                            }
                        }
                    }
                }
                closedir(procDir);
                
                // Sort processes by memory usage (descending)
                std::sort(processList.begin(), processList.end(), 
                    [](const auto& a, const auto& b) { 
                        return a.second > b.second; 
                    }
                );
                
                // Kill the top memory consumer if it's using significant memory
                // Be careful with this in production - it might kill important processes
                if (!processList.empty() && processList[0].second > 100000) {
                    // Check if it's not a critical system process
                    std::stringstream cmdlinePath;
                    cmdlinePath << "/proc/" << processList[0].first << "/cmdline";
                    std::ifstream cmdlineFile(cmdlinePath.str());
                    
                    if (cmdlineFile.is_open()) {
                        std::string cmdline;
                        std::getline(cmdlineFile, cmdline);
                        cmdlineFile.close();
                        
                        // Skip system processes (simplified check)
                        if (cmdline.find("systemd") == std::string::npos && 
                            cmdline.find("init") == std::string::npos) {
                            // Kill the process
                            std::stringstream killCmd;
                            killCmd << "kill -15 " << processList[0].first;
                            std::system(killCmd.str().c_str()); // Use std::system instead of system
                            
                            // Log the action
                            std::cout << "Terminated process " << processList[0].first 
                                    << " using " << processList[0].second << "KB memory" 
                                    << std::endl;
                        }
                    }
                }
            }
        }
    }
    #else
    // Mock implementation for non-Linux platforms
    // Simply log the threshold
    std::cout << "Memory optimization threshold set to " << threshold << "%" << std::endl;
    #endif
}
