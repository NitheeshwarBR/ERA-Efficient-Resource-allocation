// ...existing code...

## Genetic Algorithm Terminology Mapping

| GA Term | Definition | ERA Implementation |
|---------|------------|-------------------|
| **Chromosome** | A potential solution to the problem | A set of resource thresholds (CPU, memory, power) |
| **Gene** | A single element of the chromosome | An individual threshold value (e.g., CPU threshold of 70%) |
| **Population** | Collection of chromosomes (potential solutions) | 20 different threshold configurations evaluated simultaneously |
| **Fitness Function** | Evaluates how good a chromosome is | Measures how well thresholds match actual resource usage patterns |
| **Selection** | Process of choosing chromosomes for reproduction | Tournament selection to pick chromosomes with better fitness |
| **Crossover** | Combining parent chromosomes to create offspring | Mixing threshold values from two parent configurations |
| **Mutation** | Random changes to maintain diversity | Small random adjustments to threshold values |

### How Resource Optimization Maps to GA

Our resource optimization translates to genetic algorithm components as follows:

- **Chromosome Structure**: 
  ```cpp
  struct OptimizationParams {
      double cpu_threshold;     // Gene 1
      double memory_threshold;  // Gene 2  
      double power_threshold;   // Gene 3
  };
  ```

- **Fitness Calculation**:
  ```cpp
  fitness = 100.0 - (
      abs(cpu_threshold - current.cpu_usage) * 1.0 +
      abs(memory_threshold - current.memory_usage) * 1.0 +
      abs(power_threshold - current.power_usage) * 1.0
  )
  ```

- **Evolution Process**:
  1. Monitor current resource usage (CPU%, memory%, power watts)
  2. Evaluate fitness of each threshold configuration
  3. Select best configurations for "reproduction" (instance)
  4. Create new configurations via crossover and mutation
  5. Apply new thresholds to system
  6. Repeat process continuously to adapt to changing conditions

This genetic approach allows the system to "learn" the optimal threshold values over time, adapting to both the specific hardware capabilities and the unique usage patterns of the user.

// ...existing code...
// ...existing code...

### CPU Optimization

- **Dynamic CPU Governor Control**: Switches between "performance", "powersave", and "conservative" modes based on load
  - **Data Structure**: Directory traversal using `std::vector<std::string>` to store CPU directories
  - **Implementation**: Reads from and writes to `/sys/devices/system/cpu/*/cpufreq/scaling_governor` files
  - **Algorithm**: Simple conditional logic that selects governor based on threshold comparison
  ```cpp
  std::vector<std::string> cpuDirs;  // Stores CPU directories like "cpu0", "cpu1"
  // When CPU usage > threshold + 20.0%:
  for (const auto& cpu : cpuDirs) {
      // Set governor to "performance"
  }
  // When CPU usage > threshold:
  for (const auto& cpu : cpuDirs) {
      // Set governor to "powersave"
  }
  // Otherwise:
  for (const auto& cpu : cpuDirs) {
      // Set governor to "conservative"
  }
  ```

- **Process Priority Management**: Identifies CPU-intensive processes and reduces their priority using `renice`
  - **Data Structure**: `std::vector<std::pair<int, double>>` storing process ID and CPU usage pairs
  - **Sorting Algorithm**: STL sort with custom comparator to order by CPU usage (highest first)
  - **Implementation**: Traverses `/proc` directory, collects process stats, and renice top CPU consumers
  ```cpp
  std::vector<std::pair<int, double>> processList;  // [pid, cpu_usage%]
  // Collect process data
  // Sort processes by CPU usage (descending)
  std::sort(processList.begin(), processList.end(), 
    [](const auto& a, const auto& b) { return a.second > b.second; });
  // Adjust priority of top consumers
  for (size_t i = 0; i < std::min(size_t(3), processList.size()); ++i) {
      // renice process
  }
  ```

- **Core Frequency Scaling**: Adjusts CPU frequency to balance performance and power consumption
  - **Data Structure**: String manipulation and file I/O operations
  - **Implementation**: Writes to frequency scaling files in sysfs
  - **Algorithm**: Threshold-based decision making
  ```cpp
  // When CPU usage is particularly high or low:
  std::string freqPath = "/sys/devices/system/cpu/" + cpu + "/cpufreq/scaling_max_freq";
  std::ofstream freqFile(freqPath);
  // Write appropriate frequency value
  ```

- **Process Termination (High CPU)**: In extreme cases, terminates non-critical processes consuming excessive CPU
  - **Data Structure**: Same vector of process-usage pairs as in priority management
  - **Algorithm**: Selective termination based on CPU usage threshold and process type validation
  - **Implementation**: System call to kill command after filtering system processes
  ```cpp
  // For extreme cases (CPU usage > threshold + 20.0):
  if (!processList.empty() && processList[0].second > criticalThreshold) {
      // Verify process is not system critical (using string search)
      std::string cmdline = readProcessCommand(processList[0].first);
      if (!isSystemProcess(cmdline)) {
          // Terminate process
          std::stringstream killCmd;
          killCmd << "kill -15 " << processList[0].first;
          std::system(killCmd.str().c_str());
      }
  }
  ```

### Memory Optimization
// ...existing code...
# ERA - Efficient Resource Allocation
## Project Presentation Guide

This document provides key points for presenting the ERA system, focusing on the optimization techniques, genetic algorithm implementation, and resource threshold considerations.

## Optimization Techniques

### CPU Optimization
- **Dynamic CPU Governor Control**: Switches between "performance", "powersave", and "conservative" modes based on load - queue data structure
- **Process Priority Management**: Identifies CPU-intensive processes and reduces their priority using `renice`
- **Core Frequency Scaling**: Adjusts CPU frequency to balance performance and power consumption

### Memory Optimization
- **Cache Management**: Clears system caches when memory pressure increases via `/proc/sys/vm/drop_caches` - heap data structure
- **Memory Compaction**: Reduces memory fragmentation using `/proc/sys/vm/compact_memory`
- **Process Termination (High Memory)**: Terminates non-critical processes consuming excessive memory (>100MB) - priority Queue

### Technical Implementation Details

#### Data Structures and Algorithms

##### CPU Optimization
- **Vector-based Process Tracking**: 
  ```cpp
  std::vector<std::pair<int, double>> processList;
  ```
  - Stores process IDs and their CPU usage percentages
  - Sorted using C++ STL sort with custom comparator to identify highest CPU consumers
  - Time complexity: O(n log n) for sorting process list
  
- **Directory Tree Traversal**: 
  - Uses `dirent.h` library to traverse `/proc` filesystem
  - Identifies process-specific information with O(n) complexity where n is number of processes
  
- **CPU Time Calculation**:
  ```cpp
  std::vector<long> cpu_times;
  ```
  - Parses `/proc/stat` data to calculate CPU utilization percentage
  - Uses delta calculations between sequential readings for accurate usage metrics

##### Memory Optimization
- **Process Memory Map**:
  ```cpp
  std::vector<std::pair<int, long>> processList;
  ```
  - Maps process IDs to memory consumption (in KB)
  - Prioritizes processes by memory usage using heap-like sorting
  
- **Memory Pressure Algorithm**:
  - Calculates memory pressure using a specialized formula:
    ```cpp
    long used_memory = total_memory - free_memory - buffers - cached;
    double memory_percentage = 100.0 * (double)used_memory / (double)total_memory;
    ```
  - Implements tiered response based on severity levels (5%, 10%, 15% above threshold)

##### Power Optimization
- **Device Directory Structure**:
  - Traverses sysfs (`/sys/devices/system/cpu/` and `/sys/class/backlight/`) to access hardware controls
  - Uses string manipulation to identify appropriate control interfaces
  
- **Power Consumption Tracking**:
  - Parses battery information from `/sys/class/power_supply/`
  - Converts microwatt readings to watts with appropriate scaling

#### Implementation Architecture

##### Resource Monitoring
- **Observer Pattern**:
  - Monitors continuously poll system resources at regular intervals
  - Resource changes trigger optimization strategies based on thresholds

##### Optimization Decision Tree
- **Tiered Response System**:
  - Light optimization: Adjusts non-critical parameters (CPU governor)
  - Medium optimization: More aggressive actions (memory compaction, process priority)
  - Heavy optimization: Most aggressive actions (process termination consideration)
  
##### Thread Management
- **Concurrent Resource Handling**:
  ```cpp
  std::thread cpuLoadThread(generateCpuLoad);
  std::thread memoryLoadThread(generateMemoryLoad);
  std::thread optimizationThread(resourceOptimization);
  ```
  - Multi-threaded architecture to separate monitoring, optimization, and UI feedback
  - Thread-safe communication using atomic variables and mutex locks
  ```cpp
  std::atomic<bool> running{true};
  std::atomic<int> loadLevel{0};
  std::mutex resourceMutex;
  ```

##### Data Persistence
- **Time Series Collection**:
  ```cpp
  struct HistoryPoint {
    long timestamp;
    double cpu_usage;
    double memory_usage;
    double power_usage;
    double cpu_threshold;
    double memory_threshold;
    double power_threshold;
    double fitness;
    int generation;
  };
  std::vector<HistoryPoint> historyData;
  ```
  - Records optimization history for analysis and visualization
  - Implements circular buffer pattern with bounded size to prevent memory growth
  ```cpp
  if (historyData.size() > MAX_HISTORY_POINTS) {
      historyData.removeFirst();
  }
  ```

### Power Optimization
- **Dynamic CPU Frequency Control**: Adjusts CPU frequency based on power consumption
- **Backlight Management**: Dynamically adjusts screen brightness to reduce power consumption
- **Device Power States**: Manages power states for various components
- **Wireless Controls**: Suggests disabling wireless interfaces during critical power conditions

## Genetic Algorithm Implementation

### Core Algorithm
The ERA system uses a genetic algorithm (GA) to adaptively find optimal resource thresholds through evolutionary computation:

1. **Population Initialization**:
   - Creates a population of "chromosomes" (potential solutions)
   - Each chromosome contains three genes: CPU threshold, memory threshold, and power threshold
   - Initial values are randomized within reasonable ranges

2. **Fitness Evaluation**:
   - Each chromosome's fitness is evaluated using the formula:
     ```
     fitness = 100.0 - (
       abs(cpu_threshold - current.cpu_usage) * 1.0 +
       abs(memory_threshold - current.memory_usage) * 1.0 +
       abs(power_threshold - current.power_usage) * 2.0
     )
     ```
   - Penalties are applied if thresholds are too high (resource exhaustion) or too low (underutilization)
   - Higher fitness scores indicate better solutions (closer to optimal thresholds)

3. **Selection**:
   - Tournament selection to choose chromosomes for reproduction
   - Higher fitness chromosomes have a greater chance of being selected

4. **Crossover**:
   - Combines genes from two parent chromosomes to create offspring
   - Uses uniform crossover with random gene selection from either parent

5. **Mutation**:
   - Random alterations to maintain genetic diversity
   - Mutation rate varies dynamically based on population convergence

6. **Evolution Process**:
   - Repeats across generations, gradually improving optimization parameters
   - Elitism ensures the best solution is preserved between generations

### Adaptation to Changing Loads
- The GA continuously runs, adapting thresholds as system loads change
- Different load levels (light, medium, spike) trigger different optimization strategies
- The system learns from past optimizations to improve future decisions

## Resource Metrics and Thresholds

### CPU Usage
- **Metric Unit**: Percentage (0-100%)
- **Threshold Basis**: 
  - Determined by analyzing historical system behavior and workload patterns
  - Light load threshold: ~30-40%
  - Medium load threshold: ~50-60% 
  - High load threshold: ~70-80%
  - GA optimizes these thresholds based on actual usage patterns

### Memory Usage
- **Metric Unit**: Percentage of total available memory (0-100%)
- **Threshold Basis**:
  - Calculated as: (Used Memory / Total Memory) × 100%
  - Used Memory = Total Memory - Free Memory - Buffers - Cached
  - Light load threshold: ~50-60%
  - Medium load threshold: ~70-80%
  - High load threshold: ~85-95%
  - Critical threshold: >95% (aggressive optimization)

### Power Usage
- **Metric Unit**: Watts (W)
  - Read from system power interfaces (e.g., `/sys/class/power_supply/`)
  - Converted from microwatts to watts for readability
- **Threshold Basis**:
  - Based on typical device power profiles
  - Light usage: 2-5W
  - Medium usage: 5-10W 
  - Heavy usage: 10-15W
  - Critical threshold: >15W (aggressive power saving)

### Load Test Parameters
In our load test implementation:
- **Light Load**: 1 CPU thread, 50MB memory, 500ms interval
- **Medium Load**: 2 CPU threads, 100MB memory, 200ms interval
- **Spike Load**: 4 CPU threads, 200MB memory, 50ms interval

These metrics were chosen to provide meaningful stress on the system while allowing the genetic algorithm to meaningfully optimize resource usage.

## Presentation Demo Tips

When presenting the ERA system:

1. **Start with Light Load**: Show how the system establishes baseline thresholds
2. **Transition to Medium Load**: Demonstrate threshold adaptation
3. **Trigger Spike Load**: Highlight aggressive optimization techniques
4. **Return to Light Load**: Show the system returning to balanced operation
5. **Highlight Fitness Score**: Point out how it improves over generations

The key message: ERA uses genetic algorithms to continuously learn and adapt resource optimization strategies based on changing workloads, resulting in improved efficiency over time.
# ERA - Efficient Resource Allocation
## Project Presentation Guide

This document provides key points for presenting the ERA system, focusing on the optimization techniques, genetic algorithm implementation, and resource threshold considerations.

## Optimization Techniques

### CPU Optimization
- **Dynamic CPU Governor Control**: Switches between "performance", "powersave", and "conservative" modes based on load
- **Process Priority Management**: Identifies CPU-intensive processes and reduces their priority using `renice`
- **Core Frequency Scaling**: Adjusts CPU frequency to balance performance and power consumption
- **Process Termination (High CPU)**: In extreme cases, terminates non-critical processes consuming excessive CPU

### Memory Optimization
- **Cache Management**: Clears system caches when memory pressure increases via `/proc/sys/vm/drop_caches`
- **Memory Compaction**: Reduces memory fragmentation using `/proc/sys/vm/compact_memory`
- **Process Memory Monitoring**: Tracks memory usage by processes to identify memory-intensive applications
- **Process Termination (High Memory)**: Terminates non-critical processes consuming excessive memory (>100MB)

### Power Optimization
- **Dynamic CPU Frequency Control**: Adjusts CPU frequency based on power consumption
- **Backlight Management**: Dynamically adjusts screen brightness to reduce power consumption
- **Device Power States**: Manages power states for various components
- **Wireless Controls**: Suggests disabling wireless interfaces during critical power conditions

## Genetic Algorithm Implementation

### Core Algorithm
The ERA system uses a genetic algorithm (GA) to adaptively find optimal resource thresholds through evolutionary computation:

1. **Population Initialization**:
   - Creates a population of "chromosomes" (potential solutions)
   - Each chromosome contains three genes: CPU threshold, memory threshold, and power threshold
   - Initial values are randomized within reasonable ranges

2. **Fitness Evaluation**:
   - Each chromosome's fitness is evaluated using the formula:
     ```
     fitness = 100.0 - (
       abs(cpu_threshold - current.cpu_usage) * 1.0 +
       abs(memory_threshold - current.memory_usage) * 1.0 +
       abs(power_threshold - current.power_usage) * 2.0
     )
     ```
   - Penalties are applied if thresholds are too high (resource exhaustion) or too low (underutilization)
   - Higher fitness scores indicate better solutions (closer to optimal thresholds)

3. **Selection**:
   - Tournament selection to choose chromosomes for reproduction
   - Higher fitness chromosomes have a greater chance of being selected

4. **Crossover**:
   - Combines genes from two parent chromosomes to create offspring
   - Uses uniform crossover with random gene selection from either parent

5. **Mutation**:
   - Random alterations to maintain genetic diversity
   - Mutation rate varies dynamically based on population convergence

6. **Evolution Process**:
   - Repeats across generations, gradually improving optimization parameters
   - Elitism ensures the best solution is preserved between generations

### Adaptation to Changing Loads
- The GA continuously runs, adapting thresholds as system loads change
- Different load levels (light, medium, spike) trigger different optimization strategies
- The system learns from past optimizations to improve future decisions

## Resource Metrics and Thresholds

### CPU Usage
- **Metric Unit**: Percentage (0-100%)
- **Threshold Basis**: 
  - Determined by analyzing historical system behavior and workload patterns
  - Light load threshold: ~30-40%
  - Medium load threshold: ~50-60% 
  - High load threshold: ~70-80%
  - GA optimizes these thresholds based on actual usage patterns

### Memory Usage
- **Metric Unit**: Percentage of total available memory (0-100%)
- **Threshold Basis**:
  - Calculated as: (Used Memory / Total Memory) × 100%
  - Used Memory = Total Memory - Free Memory - Buffers - Cached
  - Light load threshold: ~50-60%
  - Medium load threshold: ~70-80%
  - High load threshold: ~85-95%
  - Critical threshold: >95% (aggressive optimization)

### Power Usage
- **Metric Unit**: Watts (W)
  - Read from system power interfaces (e.g., `/sys/class/power_supply/`)
  - Converted from microwatts to watts for readability
- **Threshold Basis**:
  - Based on typical device power profiles
  - Light usage: 2-5W
  - Medium usage: 5-10W 
  - Heavy usage: 10-15W
  - Critical threshold: >15W (aggressive power saving)

### Load Test Parameters
In our load test implementation:
- **Light Load**: 1 CPU thread, 50MB memory, 500ms interval
- **Medium Load**: 2 CPU threads, 100MB memory, 200ms interval
- **Spike Load**: 4 CPU threads, 200MB memory, 50ms interval

These metrics were chosen to provide meaningful stress on the system while allowing the genetic algorithm to meaningfully optimize resource usage.

## Presentation Demo Tips

When presenting the ERA system:

1. **Start with Light Load**: Show how the system establishes baseline thresholds
2. **Transition to Medium Load**: Demonstrate threshold adaptation
3. **Trigger Spike Load**: Highlight aggressive optimization techniques
4. **Return to Light Load**: Show the system returning to balanced operation
5. **Highlight Fitness Score**: Point out how it improves over generations

The key message: ERA uses genetic algorithms to continuously learn and adapt resource optimization strategies based on changing workloads, resulting in improved efficiency over time.
