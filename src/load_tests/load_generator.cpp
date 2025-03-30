#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <cpr/cpr.h>
#include <string>
#include <sstream>
#include <cmath>
#include <malloc.h>
#include <cstring>  // Add this for memset
#include <mutex>

// Constants for load levels
constexpr int LIGHT_LOAD_CPU_THREADS = 1;
constexpr int MEDIUM_LOAD_CPU_THREADS = 2;
constexpr int SPIKE_LOAD_CPU_THREADS = 4;

constexpr size_t LIGHT_LOAD_MEMORY_MB = 50;
constexpr size_t MEDIUM_LOAD_MEMORY_MB = 100;
constexpr size_t SPIKE_LOAD_MEMORY_MB = 200;

constexpr int LIGHT_LOAD_INTERVAL_MS = 500;
constexpr int MEDIUM_LOAD_INTERVAL_MS = 200;
constexpr int SPIKE_LOAD_INTERVAL_MS = 50;

// Global control variables
std::atomic<bool> running{true};
std::atomic<int> currentLoadLevel{0}; // 0=light, 1=medium, 2=spike
std::vector<std::thread> cpuThreads;
std::vector<char*> memoryBlocks;
std::mutex memoryMutex;

// Simple JSON parser for our specific use case
int extractLoadLevel(const std::string& jsonResponse) {
    // Find the load_level value in the JSON
    size_t pos = jsonResponse.find("\"load_level\":");
    if (pos != std::string::npos) {
        pos += 12; // Move past "load_level":
        // Skip whitespace
        while (pos < jsonResponse.length() && std::isspace(jsonResponse[pos])) {
            pos++;
        }
        // Read the integer
        if (pos < jsonResponse.length() && std::isdigit(jsonResponse[pos])) {
            return jsonResponse[pos] - '0';
        }
    }
    return 0; // Default to light load
}

// CPU intensive operation
void cpuIntensiveTask() {
    while (running) {
        // Compute heavy calculations
        double result = 0;
        for (int i = 0; i < 10000000; i++) {
            result += std::sin(i) * std::cos(i);
        }
        
        // Sleep based on load level
        int sleepTime;
        switch (currentLoadLevel.load()) {
            case 0: sleepTime = LIGHT_LOAD_INTERVAL_MS; break;
            case 1: sleepTime = MEDIUM_LOAD_INTERVAL_MS; break;
            case 2: sleepTime = SPIKE_LOAD_INTERVAL_MS; break;
            default: sleepTime = LIGHT_LOAD_INTERVAL_MS;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
    }
}

// Memory management
void manageMemory() {
    size_t targetMemoryMB;
    
    while (running) {
        // Determine target memory based on load level
        switch (currentLoadLevel.load()) {
            case 0: targetMemoryMB = LIGHT_LOAD_MEMORY_MB; break;
            case 1: targetMemoryMB = MEDIUM_LOAD_MEMORY_MB; break;
            case 2: targetMemoryMB = SPIKE_LOAD_MEMORY_MB; break;
            default: targetMemoryMB = LIGHT_LOAD_MEMORY_MB;
        }
        
        // Adjust memory usage
        std::lock_guard<std::mutex> lock(memoryMutex);
        
        // Release memory if we have too much
        while (!memoryBlocks.empty() && memoryBlocks.size() * 10 > targetMemoryMB) {
            free(memoryBlocks.back());
            memoryBlocks.pop_back();
        }
        
        // Allocate more if needed
        while (memoryBlocks.size() * 10 < targetMemoryMB) {
            char* block = (char*)malloc(10 * 1024 * 1024); // 10MB blocks
            if (block) {
                // Write to memory to ensure physical allocation
                memset(block, 1, 10 * 1024 * 1024);
                memoryBlocks.push_back(block);
            } else {
                // Out of memory, stop allocating
                break;
            }
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

// Load level monitoring
void monitorLoadLevel() {
    while (running) {
        try {
            // Send GET request to API to check load level
            cpr::Response r = cpr::Get(cpr::Url{"http://localhost:8080/api/resources"});
            
            if (r.status_code == 200) {
                int newLoadLevel = extractLoadLevel(r.text);
                if (newLoadLevel != currentLoadLevel) {
                    std::cout << "Changing load level from " 
                              << currentLoadLevel.load() << " to " 
                              << newLoadLevel << std::endl;
                    currentLoadLevel.store(newLoadLevel);
                    
                    // Adjust CPU threads based on new load level
                    std::vector<std::thread> newThreads;
                    int targetThreads;
                    
                    switch (newLoadLevel) {
                        case 0: targetThreads = LIGHT_LOAD_CPU_THREADS; break;
                        case 1: targetThreads = MEDIUM_LOAD_CPU_THREADS; break;
                        case 2: targetThreads = SPIKE_LOAD_CPU_THREADS; break;
                        default: targetThreads = LIGHT_LOAD_CPU_THREADS;
                    }
                    
                    // Kill existing threads if needed
                    if (cpuThreads.size() > targetThreads) {
                        for (auto& thread : cpuThreads) {
                            if (thread.joinable()) {
                                thread.join();
                            }
                        }
                        cpuThreads.clear();
                    }
                    
                    // Start new CPU threads
                    while (cpuThreads.size() < targetThreads) {
                        cpuThreads.emplace_back(cpuIntensiveTask);
                    }
                }
            } else {
                std::cerr << "Error getting load level: " << r.status_code << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Network error: " << e.what() << std::endl;
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() {
    std::cout << "Starting load generator..." << std::endl;
    
    // Start initial CPU threads (light load)
    for (int i = 0; i < LIGHT_LOAD_CPU_THREADS; i++) {
        cpuThreads.emplace_back(cpuIntensiveTask);
    }
    
    // Start memory management thread
    std::thread memoryThread(manageMemory);
    
    // Start load level monitor
    std::thread monitorThread(monitorLoadLevel);
    
    std::cout << "Load generator running. Press Enter to stop." << std::endl;
    std::cin.get();
    
    // Clean up
    running = false;
    
    for (auto& thread : cpuThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    if (memoryThread.joinable()) {
        memoryThread.join();
    }
    
    if (monitorThread.joinable()) {
        monitorThread.join();
    }
    
    // Free memory
    std::lock_guard<std::mutex> lock(memoryMutex);
    for (auto block : memoryBlocks) {
        free(block);
    }
    memoryBlocks.clear();
    
    std::cout << "Load generator stopped." << std::endl;
    return 0;
}
