# ERA - Efficient Resource Allocation

## Overview

ERA (Efficient Resource Allocation) is a system designed to optimize resource usage on Raspberry Pi devices using genetic algorithms. It monitors and intelligently adjusts CPU, memory, and power usage based on evolutionary algorithms that adapt to changing workloads.

### Key Features

- **Real-time Resource Monitoring**: Tracks CPU, memory, and power usage
- **Genetic Algorithm Optimization**: Evolves resource allocation strategies over time
- **Adaptive Thresholds**: Automatically adjusts resource thresholds based on workload patterns
- **Terminal-based Visualization**: Clean, ASCII-based interface for monitoring system status
- **Load Testing**: Simulates various workload scenarios to evaluate optimization efficiency

## System Architecture

The system consists of several main components:
- **Resource Monitors**: Track CPU, memory, and power consumption
- **Genetic Algorithm Engine**: Optimizes resource thresholds over generations
- **Resource Optimizers**: Apply optimized settings to the system
- **Visual Interface**: Terminal-based ASCII visualization

## Getting Started

### Prerequisites

- Raspberry Pi 3 or newer (tested on Raspberry Pi 4)
- Raspbian OS or Ubuntu 20.04+ for ARM
- CMake 3.10+
- C++17 compatible compiler

### Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/ERA-Efficient-Resource-allocation.git
   cd ERA-Efficient-Resource-allocation
   ```

2. Build the system:
   ```bash
   mkdir build && cd build
   cmake ..
   make -j4
   ```

## Running the System

1. Start the optimizer (requires root for system modifications):
   ```bash
   sudo ./optimizer
   ```

2. In a separate terminal, run the load generator to test different load levels:
   ```bash
   ./load_generator
   ```

## Using the System

### Terminal-based Interface

The main interface provides:
- **ASCII Progress Bars**: Visual representation of resource usage vs. thresholds
- **Real-time Resource Monitoring**: Current CPU, memory, and power consumption
- **Genetic Algorithm Metrics**: Displays fitness scores and optimization progress
- **Simple Controls**: Keyboard commands to change load levels and control the system

### Testing Different Loads

You can test how the system responds to different workloads by pressing:
- **0**: Light Load - Minimal resource usage
- **1**: Medium Load - Moderate resource usage
- **2**: Spike Load - Heavy resource consumption

### Interpreting Results

The interface presents two key values for each resource:
- **Actual Usage**: Current resource consumption (shown as progress bars)
- **Optimized Threshold**: The value determined by the genetic algorithm (shown as markers)

The bars change color based on the relationship between usage and thresholds:
- **Green**: Resource usage is below the optimized threshold
- **Red**: Resource usage exceeds the optimized threshold
- **Yellow Marker**: Indicates the current threshold position

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
- Adjusts CPU governor settings (performance, powersave, conservative)
- Modifies process priorities for CPU-intensive tasks
- Balances workloads across cores

#### Memory Optimization
- Clears caches and buffers when memory pressure increases
- Compacts memory allocations to reduce fragmentation
- Manages high-memory processes through priority adjustment

#### Power Optimization
- Adjusts CPU frequency scaling based on workload
- Controls device power states
- Optimizes background processes

## Development

### Project Structure

## Interpreting the Results

The terminal interface displays real-time information about system resource usage and the genetic algorithm's decisions. Here's how to interpret what you see:

### Understanding the Output