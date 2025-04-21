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
#include <chrono>
#include <vector>

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
std::atomic<bool> runOptimization{true}; // Control optimization thread

// History data for graphs
struct HistoryPoint {
    double timestamp;
    double cpu_usage;
    double memory_usage;
    double power_usage;
    double cpu_threshold;
    double memory_threshold;
    double power_threshold;
    double fitness;
};

std::vector<HistoryPoint> historyData;
std::mutex historyMutex;
const size_t MAX_HISTORY_POINTS = 120; // 2 minutes of data at 1 sample/sec

// Background optimization thread
void optimizationThread() {
    auto lastTime = std::chrono::steady_clock::now();
    
    while (runOptimization) {
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
        
        // Record history data every second
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();
        
        if (elapsedMs >= 1000) { // 1 second interval
            lastTime = currentTime;
            
            HistoryPoint point;
            point.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
                currentTime.time_since_epoch()).count();
            
            {
                std::lock_guard<std::mutex> lock(resourceMutex);
                point.cpu_usage = currentResources.cpu_usage;
                point.memory_usage = currentResources.memory_usage;
                point.power_usage = currentResources.power_usage;
                point.cpu_threshold = optimizedParams.cpu_threshold;
                point.memory_threshold = optimizedParams.memory_threshold;
                point.power_threshold = optimizedParams.power_threshold;
                point.fitness = bestChromosome.getFitness();
            }
            
            {
                std::lock_guard<std::mutex> lock(historyMutex);
                historyData.push_back(point);
                
                // Limit history size
                if (historyData.size() > MAX_HISTORY_POINTS) {
                    historyData.erase(historyData.begin());
                }
            }
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
    
    crow::SimpleApp app;
    
    // Enable CORS through response headers directly
    // (removing middleware approach that's causing compilation errors)
    
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
        crow::response resp(result.dump());
        // Add CORS headers manually
        resp.add_header("Access-Control-Allow-Origin", "*");
        resp.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE");
        resp.add_header("Access-Control-Allow-Headers", "Content-Type");
        return resp;
    });
    
    // Get history data for charts
    CROW_ROUTE(app, "/api/history")
    ([]() {
        std::lock_guard<std::mutex> lock(historyMutex);
        json result = json::array();
        
        for (const auto& point : historyData) {
            result.push_back({
                {"timestamp", point.timestamp},
                {"cpu_usage", point.cpu_usage},
                {"memory_usage", point.memory_usage},
                {"power_usage", point.power_usage},
                {"cpu_threshold", point.cpu_threshold},
                {"memory_threshold", point.memory_threshold},
                {"power_threshold", point.power_threshold},
                {"fitness", point.fitness}
            });
        }
        
        crow::response resp(result.dump());
        // Add CORS headers manually
        resp.add_header("Access-Control-Allow-Origin", "*");
        resp.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE");
        resp.add_header("Access-Control-Allow-Headers", "Content-Type");
        return resp;
    });
    
    // Set load level
    CROW_ROUTE(app, "/api/load/<int>")
    ([](int level) {
        if (level >= 0 && level <= 2) {
            currentLoadLevel.store(level);
            crow::response resp(200, "Load level set to " + std::to_string(level));
            // Add CORS headers
            resp.add_header("Access-Control-Allow-Origin", "*");
            resp.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE");
            resp.add_header("Access-Control-Allow-Headers", "Content-Type");
            return resp;
        }
        crow::response resp(400, "Invalid load level. Must be 0, 1, or 2.");
        // Add CORS headers
        resp.add_header("Access-Control-Allow-Origin", "*");
        resp.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE");
        resp.add_header("Access-Control-Allow-Headers", "Content-Type");
        return resp;
    });
    
    // CORS preflight request handler
    CROW_ROUTE(app, "/api/<path>")
    .methods("OPTIONS"_method)
    ([](const std::string& path) {
        crow::response resp;
        resp.add_header("Access-Control-Allow-Origin", "*");
        resp.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE");
        resp.add_header("Access-Control-Allow-Headers", "Content-Type");
        return resp;
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
    
    std::cout << "Starting ERA API server on port 8080..." << std::endl;
    std::cout << "REST API available at:" << std::endl;
    std::cout << "  GET /api/resources - Current resource usage" << std::endl;
    std::cout << "  GET /api/history - Historical data" << std::endl;
    std::cout << "  GET /api/load/<level> - Set load level (0, 1, or 2)" << std::endl;
    std::cout << std::endl;
    std::cout << "Qt Dashboard should connect to this server for data." << std::endl;
    
    // Start the server
    app.port(8080).multithreaded().run();
    
    // Cleanup when server exits
    runOptimization = false;
    if (optThread.joinable()) {
        optThread.join();
    }
    
    return 0;
}
