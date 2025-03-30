#include <crow.h>
#include <nlohmann/json.hpp>
#include "resource/cpu/cpu_monitor.hpp"
#include "resource/memory/memory_monitor.hpp"
#include "resource/power/power_monitor.hpp"
#include "resource/cpu/cpu_optimizer.hpp"
#include "resource/memory/memory_optimizer.hpp"
#include "resource/power/power_optimizer.hpp"
#include "genetic/population.hpp"
#include "utils/data_types.hpp"
#include <thread>
#include <memory>
#include <mutex>
#include <atomic>

using json = nlohmann::json;

// Global variables for resource management
std::unique_ptr<CPUMonitor> cpuMonitor;
std::unique_ptr<MemoryMonitor> memMonitor;
std::unique_ptr<PowerMonitor> powerMonitor;
std::unique_ptr<CPUOptimizer> cpuOptimizer;
std::unique_ptr<MemoryOptimizer> memOptimizer;
std::unique_ptr<PowerOptimizer> powerOptimizer;
std::unique_ptr<Population> population;

// Current optimization parameters and resource usage
SystemResources currentResources;
OptimizationParams optimizedParams;
std::mutex resourceMutex;
std::atomic<int> currentLoadLevel{0}; // 0=light, 1=medium, 2=spike

// Background optimization thread
void optimizationThread() {
    while (true) {
        // Get current resource usage
        {
            std::lock_guard<std::mutex> lock(resourceMutex);
            currentResources.cpu_usage = cpuMonitor->getCurrentUsage();
            currentResources.memory_usage = memMonitor->getCurrentUsage();
            currentResources.power_usage = powerMonitor->getCurrentUsage();
        }
        
        // Run genetic algorithm optimization
        population->evolve();
        Chromosome bestChromosome = population->getBestChromosome();
        
        // Apply optimizations
        {
            std::lock_guard<std::mutex> lock(resourceMutex);
            optimizedParams = bestChromosome.getParams();
            cpuOptimizer->optimize(optimizedParams.cpu_threshold);
            memOptimizer->optimize(optimizedParams.memory_threshold);
            powerOptimizer->optimize(optimizedParams.power_threshold);
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

int main() {
    // Initialize resource monitors and optimizers
    cpuMonitor = std::make_unique<CPUMonitor>();
    memMonitor = std::make_unique<MemoryMonitor>();
    powerMonitor = std::make_unique<PowerMonitor>();
    cpuOptimizer = std::make_unique<CPUOptimizer>();
    memOptimizer = std::make_unique<MemoryOptimizer>();
    powerOptimizer = std::make_unique<PowerOptimizer>();
    population = std::make_unique<Population>(20); // 20 chromosomes in population
    
    // Start optimization thread
    std::thread optThread(optimizationThread);
    optThread.detach();
    
    crow::SimpleApp app;
    
    // Get current resource usage
    CROW_ROUTE(app, "/api/resources")
    ([]() {
        std::lock_guard<std::mutex> lock(resourceMutex);
        json result = {
            {"current", {
                {"cpu", currentResources.cpu_usage},
                {"memory", currentResources.memory_usage},
                {"power", currentResources.power_usage}
            }},
            {"optimized", {
                {"cpu_threshold", optimizedParams.cpu_threshold},
                {"memory_threshold", optimizedParams.memory_threshold},
                {"power_threshold", optimizedParams.power_threshold}
            }},
            {"load_level", currentLoadLevel.load()}
        };
        return crow::response(result.dump());
    });
    
    // Set load level
    CROW_ROUTE(app, "/api/load/<int>")
    ([](int level) {
        if (level >= 0 && level <= 2) {
            currentLoadLevel.store(level);
            return crow::response(200);
        }
        return crow::response(400);
    });
    
    // Serve static files for React app (assuming build is in dashboard directory)
    CROW_ROUTE(app, "/<path>")
    ([](const std::string& path) {
        crow::response res;
        res.set_static_file_info("dashboard/" + path);
        return res;
    });
    
    // Redirect root to index.html
    CROW_ROUTE(app, "/")
    ([]() {
        crow::response res;
        res.set_static_file_info("dashboard/index.html");
        return res;
    });
    
    app.port(8080).multithreaded().run();
    
    return 0;
}
