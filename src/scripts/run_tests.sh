#!/bin/bash

# Test script for ERA project
echo "Running tests for Efficient Resource Allocation system..."

# Check if build directory exists
if [ ! -d "build" ]; then
    echo "Build directory not found. Building project first..."
    ./src/scripts/build.sh
fi

# Enter build directory
cd build

# Run unit tests
echo "Running unit tests..."
ctest -V --test-dir src/tests/unit

# Run integration tests
echo "Running integration tests..."
ctest -V --test-dir src/tests/integration

# Run a simple load test if load_generator exists
if [ -f "load_generator" ]; then
    echo "Running basic load test..."
    
    # Start API server in background
    sudo ./api_server &
    API_PID=$!
    
    # Wait for server to start
    sleep 5
    
    # Run load generator for 30 seconds with simulated loads
    timeout 30s ./load_generator &
    LOAD_PID=$!
    
    # Wait for tests to complete
    sleep 30
    
    # Clean up processes
    kill $LOAD_PID 2>/dev/null
    kill $API_PID 2>/dev/null
    sudo kill $(pgrep api_server) 2>/dev/null
    
    echo "Load test complete!"
fi

echo "All tests completed!"
