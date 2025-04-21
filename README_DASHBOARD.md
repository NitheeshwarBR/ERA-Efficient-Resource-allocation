# ERA Dashboard - Real-time Resource Optimization Monitoring

This document provides instructions on how to set up and run the ERA (Efficient Resource Allocation) dashboard to visualize real-time system optimization.

## Overview

ERA Dashboard comes in two variants:
1. **Qt Dashboard**: A native desktop application with rich visualization and controls
2. **Web Dashboard**: A browser-based interface that can be accessed from any device

Both dashboards provide real-time monitoring of:
- CPU, memory, and power usage
- Optimization thresholds determined by the genetic algorithm
- Fitness tracking over time
- Interactive load controls to test different scenarios

## Prerequisites

### For the Qt Dashboard
- Qt 5.12 or newer with QtCharts module
- C++17 compatible compiler
- CMake 3.10 or newer (for CMake build)

To install Qt dependencies on Ubuntu/Debian:
```bash
sudo apt install qt5-default qtcreator qt5-qmake libqt5charts5-dev
```

On Fedora:
```bash
sudo dnf install qt5-qtbase-devel qt5-qtcharts-devel
```

### For the Web Dashboard & API Server
- Crow (C++ web framework, automatically downloaded by CMake)
- nlohmann/json (JSON library, automatically downloaded by CMake)
- A modern web browser

## Building the Application

### Building with CMake (Recommended)
The CMake build system will automatically attempt to build both the API server and Qt dashboard if dependencies are available.

```bash
# Create and enter build directory
mkdir -p build
cd build

# Configure and build
cmake ..
make -j$(nproc)
```

### Building the Qt Dashboard Standalone
If you want to build just the Qt dashboard separately:

```bash
# Make the script executable (if needed)
chmod +x build_qt_dashboard.sh

# Run the build script
./build_qt_dashboard.sh
```

## Running the System

To experience real-time optimization, you'll need to run three components:

### 1. Start the API Server (required)
The API server runs the optimization engine and provides data to both dashboards.

```bash
# From the build directory
sudo ./api_server
```

The server will start on port 8080 and display available endpoints.

### 2. Start the Qt Dashboard (option 1)
For the best experience with rich visualization:

```bash
# From the build directory if built with CMake
./qt_dashboard

# Or from the qt_build directory if built separately
cd qt_build
./era_qt_dashboard
```

### 3. Access the Web Dashboard (option 2)
If you prefer a browser-based interface:

1. Open your web browser
2. Navigate to http://localhost:8080
3. The dashboard should load automatically if the API server is running

### 4. Start the Load Generator
To simulate varying workloads on the system:

```bash
# From the build directory
./load_generator
```

This will create CPU and memory loads that the optimization engine will respond to.

## Dashboard Features

### Real-time Resource Monitoring
Both dashboards display:
- Current CPU, memory, and power usage with color-coded indicators
- Threshold values determined by the genetic algorithm
- Historical charts showing resource usage trends

### Load Control
Use the "Load Control" section to switch between:
- **Light Load**: Minimal resource usage
- **Medium Load**: Moderate resource usage
- **Spike Load**: Heavy resource consumption

After changing the load level, observe how the system adapts over time:
1. Resource usage will change based on the load level
2. The genetic algorithm will evolve the thresholds to optimize performance
3. Fitness score will reflect how well the system is adapting

### Fitness Tracking
The "Genetic Algorithm Fitness" chart shows how well the system is optimizing:
- Higher values indicate better optimization
- Watch for fitness improvements as the system runs longer
- Observe how fitness responds to load changes

## Interpreting the Results

### Understanding Threshold Adaptation
- Thresholds represent where the system should trigger optimizations
- If usage exceeds thresholds (red in progress bars), optimizations are active
- Over time, thresholds should adapt to common usage patterns for better efficiency

### Optimization Strategies
When resources exceed thresholds, the system applies various optimizations:
- CPU: Adjusts CPU governor settings and process priorities
- Memory: Clears caches and adjusts memory-hungry processes
- Power: Controls power states and features to reduce consumption

### Successful Optimization
Signs that the system is optimizing effectively:
1. Fitness score trends upward over time
2. Thresholds settle near typical usage levels after initial fluctuations
3. Resource usage stays below thresholds more frequently during stable loads

## Troubleshooting

### Dashboard Not Connecting
- Verify the API server is running (should show "Starting ERA API server" message)
- Check if port 8080 is available or if another service is using it
- For Qt dashboard, confirm the server URL is correct (default: http://localhost:8080)

### No Data Displayed
- Check for error messages in the API server terminal
- Ensure the necessary permissions for resource monitoring (API server needs sudo)
- Verify network connectivity between dashboard and server

### Building Issues
- Make sure all required dependencies are installed
- For Qt errors, verify Qt5 with Charts module is properly installed
- Check CMake version is 3.10 or newer

## Further Development

The dashboard is designed to be extensible. Possible enhancements include:
- Adding support for additional resources (network, storage, etc.)
- Implementing more optimization strategies
- Creating alert systems for resource thresholds
- Supporting remote monitoring of multiple devices

For more information on the underlying optimization algorithms, see the main project README.
