#include <gtest/gtest.h>
#include "resource/cpu/cpu_monitor.hpp"
#include "resource/memory/memory_monitor.hpp"
#include "resource/power/power_monitor.hpp"
#include "genetic/population.hpp"
#include <thread>
#include <chrono>
#include <cmath>  // Add this for sin/cos functions

// Test fixture for system integration tests
class SystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code that will be called before each test
    }

    void TearDown() override {
        // Cleanup code that will be called after each test
    }
};

// Test resource monitors
TEST_F(SystemTest, ResourceMonitorsWork) {
    CPUMonitor cpuMonitor;
    MemoryMonitor memMonitor;
    PowerMonitor powerMonitor;
    
    // Check CPU monitoring
    double cpuUsage = cpuMonitor.getCurrentUsage();
    EXPECT_GE(cpuUsage, 0.0);
    EXPECT_LE(cpuUsage, 100.0);
    
    // Check memory monitoring
    double memUsage = memMonitor.getCurrentUsage();
    EXPECT_GE(memUsage, 0.0);
    EXPECT_LE(memUsage, 100.0);
    
    // Check power monitoring
    double powerUsage = powerMonitor.getCurrentUsage();
    EXPECT_GE(powerUsage, 0.0);
    EXPECT_LE(powerUsage, 30.0);  // Most Raspberry Pi units use less than 15W
}

// Test genetic algorithm
TEST_F(SystemTest, GeneticAlgorithmWorks) {
    // Create population
    Population population(10);
    
    // Run a few generations of evolution
    for (int i = 0; i < 5; i++) {
        population.evolve();
    }
    
    // Get best chromosome
    Chromosome bestChromosome = population.getBestChromosome();
    
    // Check that parameters are in reasonable ranges
    OptimizationParams params = bestChromosome.getParams();
    
    EXPECT_GE(params.cpu_threshold, 10.0);
    EXPECT_LE(params.cpu_threshold, 95.0);
    
    EXPECT_GE(params.memory_threshold, 20.0);
    EXPECT_LE(params.memory_threshold, 95.0);
    
    EXPECT_GE(params.power_threshold, 1.0);
    EXPECT_LE(params.power_threshold, 15.0);
}

// Test resource optimization under load
TEST_F(SystemTest, OptimizationUnderLoad) {
    // Create resource monitors
    CPUMonitor cpuMonitor;
    MemoryMonitor memMonitor;
    PowerMonitor powerMonitor;
    
    // Record initial resource usage
    double initialCpuUsage = cpuMonitor.getCurrentUsage();
    double initialMemUsage = memMonitor.getCurrentUsage();
    double initialPowerUsage = powerMonitor.getCurrentUsage();
    
    // Create a CPU-intensive task
    auto cpuTask = []() {
        double result = 0;
        for (int i = 0; i < 100000000; i++) {
            // Use sin and cos directly (not from std namespace)
            result += sin(i) * cos(i);
        }
        return result;
    };
    
    // Start the task in a separate thread
    std::thread taskThread(cpuTask);
    
    // Wait for resource usage to increase
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // Measure resource usage under load
    double loadCpuUsage = cpuMonitor.getCurrentUsage();
    double loadMemUsage = memMonitor.getCurrentUsage();
    double loadPowerUsage = powerMonitor.getCurrentUsage();
    
    // CPU usage should increase under load
    EXPECT_GT(loadCpuUsage, initialCpuUsage);
    
    // Join the thread
    if (taskThread.joinable()) {
        taskThread.join();
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
