# ERA - Efficient Resource Allocation

## Overview

ERA (Efficient Resource Allocation) is a system designed to optimize resource usage on Raspberry Pi devices using genetic algorithms. It monitors and intelligently adjusts CPU, memory, and power usage based on evolutionary algorithms that adapt to changing workloads.

![ERA Dashboard](docs/images/dashboard.png)

### Key Features

- **Real-time Resource Monitoring**: Tracks CPU, memory, and power usage
- **Genetic Algorithm Optimization**: Evolves resource allocation strategies over time
- **Adaptive Thresholds**: Automatically adjusts resource thresholds based on workload patterns
- **Interactive Dashboard**: React-based UI to visualize and control the system
- **Load Testing**: Simulates various workload scenarios to evaluate optimization efficiency

## System Architecture

![System Architecture](docs/images/architecture.png)

The system consists of several main components:
- **Resource Monitors**: Track CPU, memory, and power consumption
- **Genetic Algorithm Engine**: Optimizes resource thresholds
- **Resource Optimizers**: Apply optimized settings to the system
- **API Server**: Provides a REST interface to the dashboard
- **React Dashboard**: Visualizes performance and provides controls

## Getting Started

### Prerequisites

- Raspberry Pi 3 or newer (tested on Raspberry Pi 4)
- Raspbian OS or Ubuntu 20.04+ for ARM
- CMake 3.10+
- C++17 compatible compiler
- Node.js 14+ (for the dashboard)
- npm 6+ (for the dashboard)

### Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/ERA-Efficient-Resource-allocation.git
   cd ERA-Efficient-Resource-allocation
   ```

2. Build the C++ components:
   ```bash
   mkdir build && cd build
   cmake ..
   make -j4
   ```

3. Install and build the dashboard:
   ```bash
   cd dashboard
   npm install
   npm run build
   ```

4. Copy the dashboard build to the API server's static files directory:
   ```bash
   cp -r build/* ../build/dashboard/
   ```

## Running the System

1. Start the API server (requires root for resource optimization):
   ```bash
   cd build
   sudo ./api_server
   ```

2. Access the dashboard in your browser:
   ```
   http://localhost:8080
   ```

## Using the Dashboard

### Resource Monitoring

The main dashboard displays real-time metrics for:
- **CPU Usage**: Current percentage and optimization threshold
- **Memory Usage**: Current percentage and optimization threshold
- **Power Usage**: Current watts and optimization threshold

### Testing Different Loads

You can simulate different workloads using the load generator buttons:

1. **Light Load**: Minimal resource usage
2. **Medium Load**: Moderate resource usage
3. **Spike Load**: Heavy resource consumption

To start the load generator:
```bash
cd build
./load_generator
```

### Interpreting Results

The dashboard shows two sets of values for each resource:
- **Actual System Usage**: What the system is currently using
- **Optimized Threshold**: The value determined by the genetic algorithm

The efficiency of the optimization can be observed through:
- The gap between actual usage and optimized thresholds
- The system's stability during different load scenarios
- Resource usage trends over time

## How It Works

### Genetic Algorithm

The genetic algorithm optimizes resource allocation through:

1. **Population**: Maintains a set of "chromosomes" representing different resource allocation strategies
2. **Fitness Evaluation**: Rates each strategy based on resource efficiency
3. **Crossover**: Combines successful strategies to create improved ones
4. **Mutation**: Introduces random variations to explore new possibilities
5. **Evolution**: Iteratively improves strategies over generations

### Resource Optimization

When system resources exceed optimized thresholds, the system applies various techniques:

#### CPU Optimization
- Adjusts CPU governor settings
- Modifies process priorities
- Balances workloads across cores

#### Memory Optimization
- Clears caches and buffers
- Compacts memory allocations
- Manages high-memory processes

#### Power Optimization
- Adjusts CPU frequency scaling
- Controls device power states
- Optimizes background processes

## Development

### Project Structure