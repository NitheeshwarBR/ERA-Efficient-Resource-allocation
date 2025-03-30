#!/bin/bash

# Build script for ERA project
echo "Building Efficient Resource Allocation system..."

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Run CMake to configure the project
echo "Configuring project with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
echo "Building project..."
make -j$(nproc)

# Check if dashboard directory exists
if [ -d "../dashboard" ]; then
    echo "Building React dashboard..."
    cd ../dashboard
    
    # Check if npm is installed
    if command -v npm &> /dev/null; then
        # Install dependencies and build the dashboard
        npm install
        npm run build
        
        # Copy the build to the correct location
        echo "Copying dashboard files to server directory..."
        mkdir -p ../build/dashboard
        cp -r build/* ../build/dashboard/
    else
        echo "Warning: npm not found. Skipping dashboard build."
    fi
    
    cd ../build
fi

echo "Build complete! You can find the executables in the build directory."
echo ""
echo "To run the system:"
echo "  sudo ./api_server       # Starts the API server and optimization engine"
echo "  ./load_generator        # Runs load testing (in a separate terminal)"
echo ""
echo "Then open http://localhost:8080 in your browser to access the dashboard."
