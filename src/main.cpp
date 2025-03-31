#include "resource/cpu/cpu_monitor.hpp"
#include "resource/memory/memory_monitor.hpp"
#include "resource/power/power_monitor.hpp"
#include "resource/cpu/cpu_optimizer.hpp"
#include "resource/memory/memory_optimizer.hpp"
#include "resource/power/power_optimizer.hpp"
#include "genetic/population.hpp"
#include "utils/logger.hpp"
#include <thread>
#include <chrono>
#include <iostream>
#include <mutex>
#include <atomic>
#include <csignal>
#include <fstream>
#include <string>
#include <iomanip>
#include <cmath>

// Global variables for proper shutdown and load control
std::atomic<bool> running{true};
std::atomic<int> loadLevel{0}; // 0=light, 1=medium, 2=spike
std::mutex logMutex;

// Signal handler for graceful termination
void signalHandler(int signal) {
    std::cout << "Received signal " << signal << ", shutting down..." << std::endl;
    running = false;
}

// Function to generate CPU load based on the current load level
void generateCpuLoad() {
    std::cout << "Starting CPU load generator..." << std::endl;
    
    while (running) {
        int iterations;
        
        // Set workload based on current load level
        switch (loadLevel.load()) {
            case 0: // Light load
                iterations = 10000000;
                break;
            case 1: // Medium load
                iterations = 50000000;
                break;
            case 2: // Spike load
                iterations = 200000000;
                break;
            default:
                iterations = 10000000;
        }
        
        // CPU-intensive calculation
        volatile double result = 0.0;
        for (int i = 0; i < iterations; ++i) {
            result += std::sin(i) * std::cos(i);
        }
        
        // Sleep between loads to allow system response
        switch (loadLevel.load()) {
            case 0: // Light load
                std::this_thread::sleep_for(std::chrono::milliseconds(800));
                break;
            case 1: // Medium load
                std::this_thread::sleep_for(std::chrono::milliseconds(400));
                break;
            case 2: // Spike load
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                break;
            default:
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
    
    std::cout << "CPU load generator stopped." << std::endl;
}

// Function to generate memory load based on the current load level
void generateMemoryLoad() {
    std::cout << "Starting memory load generator..." << std::endl;
    
    std::vector<std::vector<char>> memoryBlocks;
    
    while (running) {
        size_t blockSize;
        size_t numBlocks;
        
        // Set memory allocation based on current load level
        switch (loadLevel.load()) {
            case 0: // Light load
                blockSize = 1024 * 1024; // 1MB
                numBlocks = 10;
                break;
            case 1: // Medium load
                blockSize = 1024 * 1024; // 1MB
                numBlocks = 30;
                break;
            case 2: // Spike load
                blockSize = 1024 * 1024; // 1MB
                numBlocks = 80;
                break;
            default:
                blockSize = 1024 * 1024; // 1MB
                numBlocks = 10;
        }
        
        // Allocate memory blocks
        try {
            memoryBlocks.clear();
            for (size_t i = 0; i < numBlocks; ++i) {
                std::vector<char> block(blockSize, 'x');
                memoryBlocks.push_back(std::move(block));
                
                // Touch the memory to ensure it's actually allocated
                for (size_t j = 0; j < blockSize; j += 4096) {
                    memoryBlocks.back()[j] = 'x';
                }
            }
        } catch (const std::bad_alloc&) {
            std::cerr << "Memory allocation failed" << std::endl;
        }
        
        // Hold the memory for a while
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    // Clear memory before exiting
    memoryBlocks.clear();
    
    std::cout << "Memory load generator stopped." << std::endl;
}

// Function for resource monitoring and optimization using genetic algorithm
void resourceOptimization() {
    // Initialize monitors
    CPUMonitor cpuMonitor;
    MemoryMonitor memMonitor;
    PowerMonitor powerMonitor;
    
    // Initialize optimizers
    CPUOptimizer cpuOptimizer;
    MemoryOptimizer memOptimizer;
    PowerOptimizer powerOptimizer;
    
    // Initialize genetic algorithm population
    Population population(20); // 20 individuals
    
    std::cout << "Starting resource optimization..." << std::endl;
    
    // Initial generation count
    int generation = 0;
    
    // Open CSV file for logging results
    std::ofstream csvFile("data/optimization_results.csv");
    if (csvFile.is_open()) {
        // Write CSV header
        csvFile << "Generation,LoadLevel,CPU_Usage,Memory_Usage,Power_Usage,"
                << "CPU_Threshold,Memory_Threshold,Power_Threshold,Fitness" << std::endl;
    }
    
    while (running) {
        try {
            // Get current resource usage
            double cpuUsage = cpuMonitor.getCurrentUsage();
            double memoryUsage = memMonitor.getCurrentUsage();
            double powerUsage = powerMonitor.getCurrentUsage();
            
            // Create system resources object
            SystemResources currentResources = {
                cpuUsage,
                memoryUsage,
                powerUsage
            };
            
            // Evolve population based on current resource usage
            population.evolve();
            generation++;
            
            // Get the best chromosome (optimization parameters)
            Chromosome bestChromosome = population.getBestChromosome();
            OptimizationParams params = bestChromosome.getParams();
            
            // Apply optimizations
            cpuOptimizer.optimize(params.cpu_threshold);
            memOptimizer.optimize(params.memory_threshold);
            powerOptimizer.optimize(params.power_threshold);
            
            // Prepare log message
            std::string log_entry = 
                "=== Generation " + std::to_string(generation) + " (Load Level: " + 
                std::to_string(loadLevel.load()) + ") ===\n" +
                "Resource Usage:\n" +
                "  CPU: " + std::to_string(cpuUsage) + "%\n" +
                "  Memory: " + std::to_string(memoryUsage) + "%\n" +
                "  Power: " + std::to_string(powerUsage) + "W\n" +
                "Optimized Thresholds:\n" +
                "  CPU: " + std::to_string(params.cpu_threshold) + "%\n" +
                "  Memory: " + std::to_string(params.memory_threshold) + "%\n" +
                "  Power: " + std::to_string(params.power_threshold) + "W\n" +
                "Fitness: " + std::to_string(bestChromosome.getFitness()) + "\n";
            
            // Log optimization results
            {
                std::lock_guard<std::mutex> lock(logMutex);
                Logger::log("data/optimization_logs.txt", log_entry);
                
                // Log to CSV for easier analysis
                if (csvFile.is_open()) {
                    csvFile << generation << ","
                            << loadLevel.load() << ","
                            << std::fixed << std::setprecision(2) << cpuUsage << ","
                            << std::fixed << std::setprecision(2) << memoryUsage << ","
                            << std::fixed << std::setprecision(2) << powerUsage << ","
                            << std::fixed << std::setprecision(2) << params.cpu_threshold << ","
                            << std::fixed << std::setprecision(2) << params.memory_threshold << ","
                            << std::fixed << std::setprecision(2) << params.power_threshold << ","
                            << std::fixed << std::setprecision(4) << bestChromosome.getFitness()
                            << std::endl;
                }
            }
            
            // Clear screen and display current state using ASCII art for visualization
            std::cout << "\033[H\033[2J";  // Clear screen
            std::cout << "\033[1;36m"; // Cyan, bold text for header
            std::cout << R"(
 _____ ____    _    
| ____|  _ \  / \   
|  _| | |_) |/ _ \  
| |___|  _ </ ___ \ 
|_____|_| \_\_/  \_\
                    
Efficient Resource Allocation System
)" << "\033[0m" << std::endl;
            
            std::cout << "═════════════════════════════════════════════\n";
            std::cout << "\033[1;33m▶ Load Level: \033[0m";
                      
            switch (loadLevel.load()) {
                case 0: std::cout << "\033[1;32mLIGHT\033[0m\n"; break;    // Green
                case 1: std::cout << "\033[1;33mMEDIUM\033[0m\n"; break;   // Yellow
                case 2: std::cout << "\033[1;31mSPIKE\033[0m\n"; break;    // Red
                default: std::cout << "UNKNOWN\n";
            }
            
            std::cout << "\033[1;33m▶ Generation: \033[0m" << generation << "\n\n";
            
            // Progress bars for resource usage
            auto createProgressBar = [](double percentage, double threshold, const std::string& label) {
                const int barWidth = 30;
                int position = static_cast<int>(percentage * barWidth / 100.0);
                int thresholdPos = static_cast<int>(threshold * barWidth / 100.0);
                
                std::stringstream bar;
                bar << label << " [";
                
                for (int i = 0; i < barWidth; ++i) {
                    if (i < position) {
                        // Color based on usage relative to threshold
                        if (percentage > threshold)
                            bar << "\033[1;31m█\033[0m"; // Red if over threshold
                        else
                            bar << "\033[1;32m█\033[0m"; // Green if under threshold
                    } else if (i == thresholdPos) {
                        bar << "\033[1;33m|\033[0m"; // Yellow threshold marker
                    } else {
                        bar << " ";
                    }
                }
                
                bar << "] " << std::fixed << std::setprecision(1) << percentage << "% ";
                bar << "(Threshold: " << std::fixed << std::setprecision(1) << threshold << "%)";
                return bar.str();
            };
            
            // Resource usage section
            std::cout << "\033[1;36m▣ Resource Usage:\033[0m\n";
            
            // Render CPU usage bar
            std::cout << createProgressBar(cpuUsage, params.cpu_threshold, "CPU   ") << "\n";
            
            // Render Memory usage bar
            std::cout << createProgressBar(memoryUsage, params.memory_threshold, "Memory") << "\n";
            
            // Render Power usage
            std::cout << "Power  [";
            const int powerBarWidth = 30;
            int powerPosition = static_cast<int>(powerUsage * powerBarWidth / 15.0); // Assuming 15W max
            int powerThresholdPos = static_cast<int>(params.power_threshold * powerBarWidth / 15.0);
            
            for (int i = 0; i < powerBarWidth; ++i) {
                if (i < powerPosition) {
                    if (powerUsage > params.power_threshold)
                        std::cout << "\033[1;31m█\033[0m"; // Red if over threshold
                    else
                        std::cout << "\033[1;32m█\033[0m"; // Green if under threshold
                } else if (i == powerThresholdPos) {
                    std::cout << "\033[1;33m|\033[0m"; // Yellow threshold marker
                } else {
                    std::cout << " ";
                }
            }
            
            std::cout << "] " << std::fixed << std::setprecision(2) << powerUsage << "W ";
            std::cout << "(Threshold: " << std::fixed << std::setprecision(2) << params.power_threshold << "W)\n\n";
            
            // Genetic algorithm section
            std::cout << "\033[1;36m▣ Genetic Algorithm Status:\033[0m\n";
            std::cout << "  Fitness Score: " << std::fixed << std::setprecision(4) 
                      << bestChromosome.getFitness() << "\n\n";
            
            // Evolution history (show last 5 generations)
            if (generation > 1) {
                std::cout << "\033[1;36m▣ Optimization Progress:\033[0m\n";
                // Here you would insert code to track and display history
                // but for simplicity we'll skip implementing that
            }
            
            std::cout << "\033[1;36m▣ Commands:\033[0m\n";
            std::cout << "  0: Light Load    1: Medium Load    2: Spike Load    q: Quit\n";
            std::cout << "═════════════════════════════════════════════\n";
            
        } catch (const std::exception& e) {
            std::cerr << "Error in resource optimization: " << e.what() << std::endl;
        }
        
        // Optimize at 2Hz
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    // Close CSV file
    if (csvFile.is_open()) {
        csvFile.close();
    }
    
    std::cout << "Resource optimization stopped." << std::endl;
}

// User input thread to change load levels
void handleUserInput() {
    std::cout << "Press 0 for light load, 1 for medium load, 2 for spike load, q to quit" << std::endl;
    
    while (running) {
        char input;
        std::cin >> input;
        
        switch (input) {
            case '0':
                loadLevel.store(0);
                std::cout << "Switched to LIGHT load" << std::endl;
                break;
            case '1':
                loadLevel.store(1);
                std::cout << "Switched to MEDIUM load" << std::endl;
                break;
            case '2':
                loadLevel.store(2);
                std::cout << "Switched to SPIKE load" << std::endl;
                break;
            case 'q':
            case 'Q':
                running = false;
                std::cout << "Shutting down..." << std::endl;
                break;
            default:
                std::cout << "Invalid input. Use 0, 1, 2 for load levels or q to quit" << std::endl;
        }
    }
}

int main() {
    // Register signal handler for graceful termination
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    
    std::cout << "Starting ERA - Efficient Resource Allocation System" << std::endl;
    std::cout << "This system will demonstrate resource optimization using genetic algorithms" << std::endl;
    std::cout << "You can switch between different load levels to see how the system adapts" << std::endl;
    
    // Create data directory if it doesn't exist
    std::system("mkdir -p data");
    
    // Start load generation threads
    std::thread cpuLoadThread(generateCpuLoad);
    std::thread memoryLoadThread(generateMemoryLoad);
    
    // Start resource optimization thread
    std::thread optimizationThread(resourceOptimization);
    
    // Handle user input in the main thread
    handleUserInput();
    
    // Wait for threads to complete (when running becomes false)
    if (cpuLoadThread.joinable()) cpuLoadThread.join();
    if (memoryLoadThread.joinable()) memoryLoadThread.join();
    if (optimizationThread.joinable()) optimizationThread.join();
    
    std::cout << "ERA system shut down successfully." << std::endl;
    std::cout << "Results have been saved to data/optimization_results.csv" << std::endl;
    std::cout << "You can analyze this data to see how the genetic algorithm adapted to different loads" << std::endl;
    
    return 0;
}
