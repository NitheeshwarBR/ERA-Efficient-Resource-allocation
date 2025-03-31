#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <string>
#include <sstream>
#include <cmath>
#include <malloc.h>
#include <cstring>
#include <mutex>
#include <csignal>


//cpu 
// thread : 1 
// memory:50mb
// interval : 500ms



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

// Signal handler for graceful termination
void signalHandler(int signal) {
    std::cout << "Received signal " << signal << ", shutting down..." << std::endl;
    running = false;
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

// User input thread to change load levels
void handleUserInput() {
    std::cout << "\033[1;36m"; // Cyan, bold text for header
    std::cout << R"(
 _____ ____    _    
| ____|  _ \  / \   
|  _| | |_) |/ _ \  
| |___|  _ </ ___ \ 
|_____|_| \_\_/  \_\
                    
Load Generator
)" << "\033[0m" << std::endl;
    
    std::cout << "Press 0 for light load, 1 for medium load, 2 for spike load, q to quit" << std::endl;
    
    while (running) {
        char input;
        std::cin >> input;
        
        switch (input) {
            case '0':
                currentLoadLevel.store(0);
                std::cout << "\033[1;32mSwitched to LIGHT load\033[0m" << std::endl;
                
                // Update thread count if needed
                while (cpuThreads.size() > LIGHT_LOAD_CPU_THREADS) {
                    std::cout << "Reducing CPU threads from " << cpuThreads.size() 
                              << " to " << LIGHT_LOAD_CPU_THREADS << std::endl;
                    // We need to stop all threads and create new ones
                    for (auto& thread : cpuThreads) {
                        if (thread.joinable()) {
                            thread.join();
                        }
                    }
                    cpuThreads.clear();
                    for (int i = 0; i < LIGHT_LOAD_CPU_THREADS; i++) {
                        cpuThreads.emplace_back(cpuIntensiveTask);
                    }
                    break;
                }
                
                // Add threads if needed
                while (cpuThreads.size() < LIGHT_LOAD_CPU_THREADS) {
                    std::cout << "Adding CPU threads to reach " << LIGHT_LOAD_CPU_THREADS << std::endl;
                    cpuThreads.emplace_back(cpuIntensiveTask);
                }
                break;
                
            case '1':
                currentLoadLevel.store(1);
                std::cout << "\033[1;33mSwitched to MEDIUM load\033[0m" << std::endl;
                
                // Update thread count
                if (cpuThreads.size() > MEDIUM_LOAD_CPU_THREADS) {
                    std::cout << "Reducing CPU threads from " << cpuThreads.size() 
                              << " to " << MEDIUM_LOAD_CPU_THREADS << std::endl;
                    // We need to stop all threads and create new ones
                    for (auto& thread : cpuThreads) {
                        if (thread.joinable()) {
                            thread.join();
                        }
                    }
                    cpuThreads.clear();
                    for (int i = 0; i < MEDIUM_LOAD_CPU_THREADS; i++) {
                        cpuThreads.emplace_back(cpuIntensiveTask);
                    }
                }
                
                // Add threads if needed
                while (cpuThreads.size() < MEDIUM_LOAD_CPU_THREADS) {
                    std::cout << "Adding CPU threads to reach " << MEDIUM_LOAD_CPU_THREADS << std::endl;
                    cpuThreads.emplace_back(cpuIntensiveTask);
                }
                break;
                
            case '2':
                currentLoadLevel.store(2);
                std::cout << "\033[1;31mSwitched to SPIKE load\033[0m" << std::endl;
                
                // Update thread count
                if (cpuThreads.size() > SPIKE_LOAD_CPU_THREADS) {
                    std::cout << "Reducing CPU threads from " << cpuThreads.size() 
                              << " to " << SPIKE_LOAD_CPU_THREADS << std::endl;
                    // We need to stop all threads and create new ones
                    for (auto& thread : cpuThreads) {
                        if (thread.joinable()) {
                            thread.join();
                        }
                    }
                    cpuThreads.clear();
                    for (int i = 0; i < SPIKE_LOAD_CPU_THREADS; i++) {
                        cpuThreads.emplace_back(cpuIntensiveTask);
                    }
                }
                
                // Add threads if needed
                while (cpuThreads.size() < SPIKE_LOAD_CPU_THREADS) {
                    std::cout << "Adding CPU threads to reach " << SPIKE_LOAD_CPU_THREADS << std::endl;
                    cpuThreads.emplace_back(cpuIntensiveTask);
                }
                break;
                
            case 'q':
            case 'Q':
                running = false;
                std::cout << "Shutting down..." << std::endl;
                break;
                
            default:
                std::cout << "Invalid input. Use 0, 1, 2 for load levels or q to quit" << std::endl;
        }
        
        // Display current status
        std::cout << "\nCurrent Status:\n";
        std::cout << "  Load Level: ";
        switch (currentLoadLevel.load()) {
            case 0: std::cout << "\033[1;32mLIGHT\033[0m"; break;
            case 1: std::cout << "\033[1;33mMEDIUM\033[0m"; break;
            case 2: std::cout << "\033[1;31mSPIKE\033[0m"; break;
        }
        std::cout << "\n  Active CPU Threads: " << cpuThreads.size() << "\n";
        std::cout << "  Memory Target: ";
        switch (currentLoadLevel.load()) {
            case 0: std::cout << LIGHT_LOAD_MEMORY_MB; break;
            case 1: std::cout << MEDIUM_LOAD_MEMORY_MB; break;
            case 2: std::cout << SPIKE_LOAD_MEMORY_MB; break;
        }
        std::cout << " MB\n\n";
    }
}

int main() {
    // Register signal handler for graceful termination
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    
    std::cout << "Starting load generator..." << std::endl;
    
    // Start initial CPU threads (light load)
    for (int i = 0; i < LIGHT_LOAD_CPU_THREADS; i++) {
        cpuThreads.emplace_back(cpuIntensiveTask);
    }
    
    // Start memory management thread
    std::thread memoryThread(manageMemory);
    
    // Handle user input for load changes
    handleUserInput();
    
    // Clean up
    std::cout << "Shutting down load generator..." << std::endl;
    running = false;
    
    for (auto& thread : cpuThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    if (memoryThread.joinable()) {
        memoryThread.join();
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
