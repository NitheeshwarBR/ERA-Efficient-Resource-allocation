#include "resource/cpu/cpu_optimizer.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <dirent.h>
#include <cstdlib> // Required for system() function
#include <thread>
#include <chrono>

void CPUOptimizer::optimize(double threshold) {
    #ifdef __linux__
    // Read current CPU usage (similar to CPUMonitor)
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
    
    // If CPU usage is above threshold, optimize
    if (cpu_percentage > threshold) {
        // Step 1: Find CPU frequency scaling directories
        DIR *dir;
        struct dirent *ent;
        std::vector<std::string> cpuDirs;
        
        if ((dir = opendir("/sys/devices/system/cpu/")) != NULL) {
            while ((ent = readdir(dir)) != NULL) {
                std::string dirName = ent->d_name;
                if (dirName.find("cpu") != std::string::npos && 
                    dirName.find("cpuidle") == std::string::npos &&
                    dirName.find("cpufreq") == std::string::npos) {
                    // Extract the number part
                    std::string numPart = dirName.substr(3);
                    bool isNumber = true;
                    for (char c : numPart) {
                        if (!std::isdigit(c)) {
                            isNumber = false;
                            break;
                        }
                    }
                    if (isNumber) {
                        cpuDirs.push_back(dirName);
                    }
                }
            }
            closedir(dir);
        }
        
        // Step 2: Adjust CPU frequency if usage is high
        if (cpu_percentage > threshold + 10.0) {
            // Set the CPU governor to powersave for balanced performance
            for (const auto& cpu : cpuDirs) {
                std::string governorPath = "/sys/devices/system/cpu/" + cpu + "/cpufreq/scaling_governor";
                std::ofstream governorFile(governorPath);
                if (governorFile.is_open()) {
                    governorFile << "powersave" << std::endl;
                    governorFile.close();
                }
            }
        } else {
            // Set the CPU governor to conservative for balanced performance
            for (const auto& cpu : cpuDirs) {
                std::string governorPath = "/sys/devices/system/cpu/" + cpu + "/cpufreq/scaling_governor";
                std::ofstream governorFile(governorPath);
                if (governorFile.is_open()) {
                    governorFile << "conservative" << std::endl;
                    governorFile.close();
                }
            }
        }
        
        // Step 3: If very high, attempt to find and kill CPU-hungry processes
        if (cpu_percentage > threshold + 20.0) {
            // Open /proc directory to scan processes
            DIR* procDir = opendir("/proc");
            if (procDir) {
                struct dirent* entry;
                std::vector<std::pair<int, double>> processList;
                
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
                        
                        // Read process CPU usage
                        std::stringstream statPath;
                        statPath << "/proc/" << pid << "/stat";
                        std::ifstream statFile(statPath.str());
                        
                        if (statFile.is_open()) {
                            std::string statLine;
                            if (std::getline(statFile, statLine)) {
                                // Parse the stat file (it's space separated)
                                std::istringstream statStream(statLine);
                                std::vector<std::string> statFields;
                                std::string field;
                                
                                while (statStream >> field) {
                                    statFields.push_back(field);
                                }
                                
                                if (statFields.size() >= 15) {
                                    // Fields 14 and 15 are utime and stime
                                    long utime = std::stol(statFields[13]);
                                    long stime = std::stol(statFields[14]);
                                    long total_time = utime + stime;
                                    
                                    // Simple CPU usage estimation
                                    double cpu_usage = (double)total_time / total_delta * 100.0;
                                    
                                    if (cpu_usage > 5.0) {  // Only include significant CPU users
                                        processList.push_back({pid, cpu_usage});
                                    }
                                }
                            }
                            statFile.close();
                        }
                    }
                }
                closedir(procDir);
                
                // Sort processes by CPU usage (descending)
                std::sort(processList.begin(), processList.end(), 
                    [](const auto& a, const auto& b) { 
                        return a.second > b.second; 
                    }
                );
                
                // Adjust process priorities (give higher nice values to CPU hogs)
                for (size_t i = 0; i < std::min(size_t(3), processList.size()); ++i) {
                    int pid = processList[i].first;
                    
                    // Read command line to avoid critical processes
                    std::stringstream cmdlinePath;
                    cmdlinePath << "/proc/" << pid << "/cmdline";
                    std::ifstream cmdlineFile(cmdlinePath.str());
                    
                    if (cmdlineFile.is_open()) {
                        std::string cmdline;
                        std::getline(cmdlineFile, cmdline);
                        cmdlineFile.close();
                        
                        // Skip system processes (simplified check)
                        if (cmdline.find("systemd") == std::string::npos && 
                            cmdline.find("init") == std::string::npos) {
                            
                            // Increase nice value (lower priority)
                            std::stringstream renice;
                            renice << "renice 10 -p " << pid;
                            // FIXED: Use std::system instead of system
                            std::system(renice.str().c_str());
                            
                            std::cout << "Lowered priority of process " << pid 
                                    << " using " << processList[i].second << "% CPU" 
                                    << std::endl;
                        }
                    }
                }
            }
        }
    } else if (cpu_percentage < threshold * 0.6) {
        // If CPU usage is well below threshold, set to performance mode
        DIR *dir;
        struct dirent *ent;
        std::vector<std::string> cpuDirs;
        
        if ((dir = opendir("/sys/devices/system/cpu/")) != NULL) {
            while ((ent = readdir(dir)) != NULL) {
                std::string dirName = ent->d_name;
                if (dirName.find("cpu") != std::string::npos && 
                    dirName.find("cpuidle") == std::string::npos &&
                    dirName.find("cpufreq") == std::string::npos) {
                    // Extract the number part
                    std::string numPart = dirName.substr(3);
                    bool isNumber = true;
                    for (char c : numPart) {
                        if (!std::isdigit(c)) {
                            isNumber = false;
                            break;
                        }
                    }
                    if (isNumber) {
                        cpuDirs.push_back(dirName);
                    }
                }
            }
            closedir(dir);
        }
        
        // Set the CPU governor to performance for each CPU
        for (const auto& cpu : cpuDirs) {
            std::string governorPath = "/sys/devices/system/cpu/" + cpu + "/cpufreq/scaling_governor";
            std::ofstream governorFile(governorPath);
            if (governorFile.is_open()) {
                governorFile << "performance" << std::endl;
                governorFile.close();
            }
        }
    }
    #else
    // Mock implementation for non-Linux platforms
    // Simply log the threshold
    std::cout << "CPU optimization threshold set to " << threshold << "%" << std::endl;
    #endif
}
