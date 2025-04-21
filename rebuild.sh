#!/bin/bash

echo "========== Cleaning and rebuilding ERA project =========="

# Clean up build directory
echo "Removing old build files..."
rm -rf build
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake ..

# Build
echo "Building project..."
make -j$(nproc)

echo "========== Build complete =========="
echo "To run the API server: sudo ./api_server"
echo "To run the Qt dashboard: ./qt_dashboard"
echo "To run the load generator: ./load_generator"
