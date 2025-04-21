# ERA Standalone Dashboard

This document provides instructions on how to set up and run the ERA (Efficient Resource Allocation) standalone dashboard application that integrates real-time monitoring and optimization in a single application.

## Overview

The standalone dashboard combines the optimization engine and visualization interface in a single application, eliminating the need for separate server and client components. This integrated approach provides:

- Direct access to system resources
- Zero network latency
- Simpler deployment and usage
- All the visualization features of the Qt dashboard

## Prerequisites

- Qt 5.12 or newer with QtCharts module
- C++17 compatible compiler
- Admin/root privileges (required for system resource monitoring)

To install Qt dependencies on Ubuntu/Debian:
```bash
sudo apt install qt5-default qtcreator qt5-qmake libqt5charts5-dev
```

On Fedora:
```bash
sudo dnf install qt5-qtbase-devel qt5-qtcharts-devel
```

## Building the Standalone Dashboard

To build the standalone dashboard:

```bash
# Make the script executable (if needed)
chmod +x build_standalone_dashboard.sh

# Run the build script
./build_standalone_dashboard.sh
```

This script will create a `standalone_build` directory and build the application there.

## Running the Standalone Dashboard

To run the standalone dashboard with full capabilities:

```bash
# Navigate to the build directory
cd standalone_build

# Run with admin privileges
sudo ./era_standalone_dashboard
```

Using sudo/admin privileges is important for accessing system resource information and applying optimizations.

## Dashboard Features

### Real-time Resource Monitoring

The dashboard displays:
- Current CPU, memory, and power usage with color-coded indicators
- Threshold values determined by the genetic algorithm
- Historical charts showing resource usage trends

### Genetic Algorithm Optimization

The integrated optimization engine:
- Monitors system resources in real-time
- Evolves optimal threshold values through genetic algorithms
- Applies resource optimizations when thresholds are exceeded
- Tracks fitness scores to measure optimization effectiveness

### Load Control

Use the "Load Control" section to switch between:
- **Light Load**: Minimal resource usage
- **Medium Load**: Moderate resource usage 
- **Spike Load**: Heavy resource consumption

After changing the load level, observe how the system adapts over time:
1. Resource usage will change based on the load level
2. The genetic algorithm will evolve the thresholds to optimize performance
3. Fitness score will reflect how well the system is adapting

## Troubleshooting

### Permission Issues
If you see warning messages about insufficient permissions:
- Make sure you're running the application with `sudo` or admin privileges
- On some systems, you may need to explicitly grant access to specific resources
- Use the provided `run_dashboard.sh` script which properly handles display settings with sudo

### X11 Display Issues with sudo
If you encounter "Could not connect to display" errors when using sudo:

1. Use the provided wrapper script:
   ```bash
   ./run_dashboard.sh
   ```

2. Or manually set the display permissions:
   ```bash
   # Allow the root user to access your X server
   xhost +local:root
   
   # Run with sudo while preserving environment variables
   sudo -E ./standalone_build/era_standalone_dashboard
   
   # Revoke root access to X server when done
   xhost -local:root
   ```

3. Alternative approach for Wayland users:
   ```bash
   # For systems using Wayland
   sudo -E QT_QPA_PLATFORM=wayland ./standalone_build/era_standalone_dashboard
   ```

### Missing Dependencies
If the application fails to start:
- Verify Qt and QtCharts are properly installed
- Check that all development libraries are installed
- Use `ldd ./era_standalone_dashboard` to check for missing libraries

### Performance Issues
If the dashboard is slow or unresponsive:
- Reduce the update frequency in the code (currently set to 1 second)
- Close other resource-intensive applications
- Check if your system meets the minimum requirements

## Load Testing

To see the optimization in action:
1. Start the dashboard
2. Run CPU or memory-intensive applications
3. Observe how the thresholds adapt over time
4. Use the integrated load level controls to simulate different workloads

## Differences from Server-Client Version

The standalone version differs from the server-client architecture in these ways:
- All components run in a single process
- No network communication is required
- Must run with admin/root privileges
- Only local monitoring is supported (no remote monitoring)
- Potentially faster response times and lower resource usage

For multi-device monitoring or scenarios where admin privileges aren't available, consider using the server-client version instead.
