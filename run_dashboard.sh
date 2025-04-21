#!/bin/bash

# This script runs the ERA standalone dashboard with proper environment settings
# when using sudo for system access permissions

# Check if the dashboard binary exists
if [ ! -f "standalone_build/era_standalone_dashboard" ]; then
    echo "Error: Dashboard binary not found."
    echo "Please build the application first using './build_standalone_dashboard.sh'"
    exit 1
fi

# Get the current user's display settings
XAUTHORITY="$HOME/.Xauthority"
DISPLAY="${DISPLAY:-:0}"

# Run the dashboard with sudo but preserve the X display environment
echo "Starting ERA dashboard with admin privileges..."
sudo -E DISPLAY="$DISPLAY" XAUTHORITY="$XAUTHORITY" standalone_build/era_standalone_dashboard

# If the above fails, try alternative methods
if [ $? -ne 0 ]; then
    echo "Alternative method: Using xhost for X11 access..."
    xhost +local:root
    sudo -E DISPLAY="$DISPLAY" XAUTHORITY="$XAUTHORITY" standalone_build/era_standalone_dashboard
    xhost -local:root
fi
