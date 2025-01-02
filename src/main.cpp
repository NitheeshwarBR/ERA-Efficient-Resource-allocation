// src/main.cpp
#include "sensors/temperature/temp_reader.hpp"
#include "sensors/mpu6050/motion_processor.hpp"
#include "sensors/camera/camera_capture.hpp"
#include "resource/cpu/cpu_monitor.hpp"
#include "resource/memory/memory_monitor.hpp"
#include "resource/power/power_monitor.hpp"
#include "genetic/population.hpp"
#include "utils/logger.hpp"
#include <thread>
#include <chrono>
#include <iostream>

void sensorDataCollection() {
    TempReader tempSensor;
    MotionProcessor mpuSensor;
    CameraCapture camSensor;
    
    while (true) {
        // Read sensors
        float temp = tempSensor.readTemperature();
        MPU6050Data mpuData = mpuSensor.readMotionData();
        CameraFrame frame = camSensor.captureFrame();
        
        // Log data using string concatenation with std::string
        std::string log_entry = std::string("Temperature: ") + std::to_string(temp) + "°C\n" +
                               std::string("MPU6050 - Accel(x,y,z): ") + 
                               std::to_string(mpuData.accelX) + "," +
                               std::to_string(mpuData.accelY) + "," +
                               std::to_string(mpuData.accelZ) + "\n" +
                               std::string("Camera frame size: ") + std::to_string(frame.size) + "\n";
        
        Logger::log("data/sensor_logs/sensor_data.txt", log_entry);
        
        // Display data
        std::cout << "\033[H\033[2J";  // Clear screen
        std::cout << log_entry;
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
void resourceOptimization() {
    CPUMonitor cpuMonitor;
    MemoryMonitor memMonitor;
    PowerMonitor powerMonitor;
    Population population(10);
    
    while (true) {
        // Get current resource usage
        SystemResources current = {
            cpuMonitor.getCurrentUsage(),
            memMonitor.getCurrentUsage(),
            powerMonitor.getCurrentUsage()
        };
        
        // Evolve population and get best parameters
        population.evolve();
        Chromosome bestChromosome = population.getBestChromosome();
        OptimizationParams params = bestChromosome.getParams();
        
        // Log results using string concatenation with std::string
        std::string log_entry = std::string("Current Usage:\n") +
                               std::string("CPU: ") + std::to_string(current.cpu_usage) + "%\n" +
                               std::string("Memory: ") + std::to_string(current.memory_usage) + "%\n" +
                               std::string("Power: ") + std::to_string(current.power_usage) + "W\n" +
                               std::string("Optimized Parameters:\n") +
                               std::string("CPU Threshold: ") + std::to_string(params.cpu_threshold) + "%\n" +
                               std::string("Memory Threshold: ") + std::to_string(params.memory_threshold) + "%\n" +
                               std::string("Power Threshold: ") + std::to_string(params.power_threshold) + "W\n";
        
        Logger::log("data/optimization_results/optimization_log.txt", log_entry);
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
int main() {
    std::thread sensorThread(sensorDataCollection);
    std::thread optimizationThread(resourceOptimization);
    
    sensorThread.join();
    optimizationThread.join();
    
    return 0;
} 