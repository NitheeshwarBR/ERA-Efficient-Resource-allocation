#include "resource/power/power_optimizer.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <dirent.h>

void PowerOptimizer::optimize(double threshold) {
    #ifdef __linux__
    // Get current power usage (using the power monitor logic)
    double currentPower = 0.0;
    
    // Try to read from /sys/class/power_supply
    std::vector<std::string> batteryDirs;
    DIR *dir;
    
    if ((dir = opendir("/sys/class/power_supply/")) != NULL) {
        struct dirent *ent;
        while ((ent = readdir(dir)) != NULL) {
            std::string dirName = ent->d_name;
            if (dirName.find("BAT") != std::string::npos) {
                batteryDirs.push_back(dirName);
            }
        }
        closedir(dir);
    }
    
    if (!batteryDirs.empty()) {
        for (const auto& battery : batteryDirs) {
            std::string powerNowPath = "/sys/class/power_supply/" + battery + "/power_now";
            std::ifstream powerFile(powerNowPath);
            if (powerFile.is_open()) {
                std::string line;
                if (std::getline(powerFile, line)) {
                    // power_now is usually in microwatts
                    currentPower += std::stod(line) / 1000000.0;  // Convert to watts
                }
                powerFile.close();
            }
        }
    }
    
    // If power usage is above threshold, optimize
    if (currentPower > threshold) {
        // Step 1: Adjust CPU frequency scaling
        // Lower the maximum CPU frequency
        std::vector<std::string> cpuDirs;
        if ((dir = opendir("/sys/devices/system/cpu/")) != NULL) {
            struct dirent *ent;
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
        
        // Set the CPU governor to powersave for each CPU
        for (const auto& cpu : cpuDirs) {
            std::string governorPath = "/sys/devices/system/cpu/" + cpu + "/cpufreq/scaling_governor";
            std::ofstream governorFile(governorPath);
            if (governorFile.is_open()) {
                governorFile << "powersave" << std::endl;
                governorFile.close();
            }
        }
        
        // Step 2: Adjust screen brightness if available
        std::string brightnessPath = "/sys/class/backlight/";
        if ((dir = opendir(brightnessPath.c_str())) != NULL) {
            struct dirent *ent;
            std::string backlightDir;
            
            // Find the first backlight device
            while ((ent = readdir(dir)) != NULL) {
                std::string dirName = ent->d_name;
                if (dirName != "." && dirName != "..") {
                    backlightDir = dirName;
                    break;
                }
            }
            closedir(dir);
            
            if (!backlightDir.empty()) {
                std::string maxBrightnessPath = brightnessPath + backlightDir + "/max_brightness";
                std::string brightnessFilePath = brightnessPath + backlightDir + "/brightness";
                
                // Read max brightness
                std::ifstream maxBrightnessFile(maxBrightnessPath);
                int maxBrightness = 100;
                if (maxBrightnessFile.is_open()) {
                    maxBrightnessFile >> maxBrightness;
                    maxBrightnessFile.close();
                }
                
                // Read current brightness
                std::ifstream currentBrightnessFile(brightnessFilePath);
                int currentBrightness = maxBrightness;
                if (currentBrightnessFile.is_open()) {
                    currentBrightnessFile >> currentBrightness;
                    currentBrightnessFile.close();
                }
                
                // Reduce brightness by 20% if above threshold by more than 2W
                if (currentPower > threshold + 2.0) {
                    int newBrightness = currentBrightness * 0.8;
                    newBrightness = std::max(newBrightness, maxBrightness / 5);  // Don't go below 20%
                    
                    std::ofstream brightnessFile(brightnessFilePath);
                    if (brightnessFile.is_open()) {
                        brightnessFile << newBrightness << std::endl;
                        brightnessFile.close();
                    }
                }
            }
        }
        
        // Step 3: Disable wireless if power usage is critical
        if (currentPower > threshold + 4.0) {
            // This is aggressive, might not want this in production without user consent
            // Use std::system instead of system
            // std::system("nmcli radio wifi off");
            std::cout << "Power usage critical, consider disabling wireless" << std::endl;
        }
    } else if (currentPower < threshold * 0.7) {
        // If power usage is well below threshold, we can relax power saving
        
        // Set the CPU governor to ondemand for each CPU
        DIR *dir;
        if ((dir = opendir("/sys/devices/system/cpu/")) != NULL) {
            struct dirent *ent;
            std::vector<std::string> cpuDirs;
            
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
            
            // Set governor to ondemand for balanced performance/power
            for (const auto& cpu : cpuDirs) {
                std::string governorPath = "/sys/devices/system/cpu/" + cpu + "/cpufreq/scaling_governor";
                std::ofstream governorFile(governorPath);
                if (governorFile.is_open()) {
                    governorFile << "ondemand" << std::endl;
                    governorFile.close();
                }
            }
        }
    }
    #else
    // Mock implementation for non-Linux platforms
    // Simply log the threshold
    std::cout << "Power optimization threshold set to " << threshold << "W" << std::endl;
    #endif
}
