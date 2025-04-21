#!/bin/bash

echo "Building ERA Qt Dashboard..."

# Check if qmake is installed
if ! command -v qmake &> /dev/null; then
    echo "Error: qmake is not installed. Please install Qt5 development packages."
    echo "On Ubuntu/Debian: sudo apt install qt5-default qtcreator qt5-qmake libqt5charts5-dev qtchooser"
    echo "On Fedora: sudo dnf install qt5-qtbase-devel qt5-qtcharts-devel qt-creator"
    exit 1
fi

# Create build directory for Qt project
mkdir -p qt_build
cd qt_build

# Run qmake
echo "Running qmake..."
qmake ../src/qt_dashboard/qt_dashboard.pro

# Build the project
echo "Building project..."
make -j$(nproc)

# Check if build was successful
if [ -f "era_qt_dashboard" ]; then
    echo "Qt Dashboard built successfully!"
    echo "To run the dashboard: ./era_qt_dashboard"
    echo "Make sure the API server is running: sudo ./api_server"
else
    echo "Build failed. Please check the error messages above."
fi
