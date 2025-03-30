#!/bin/bash

# Setup script for ERA sensors
echo "Setting up sensors for Efficient Resource Allocation system..."

# Check if running as root
if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 
   exit 1
fi

# Install required packages
echo "Installing required packages..."
apt-get update
apt-get install -y i2c-tools lm-sensors

# Enable I2C if not already enabled
if ! grep -q "^dtparam=i2c_arm=on" /boot/config.txt; then
    echo "Enabling I2C interface..."
    echo "dtparam=i2c_arm=on" >> /boot/config.txt
    echo "I2C interface enabled. A reboot will be required."
fi

# Set up temperature sensors
echo "Setting up temperature sensors..."
modprobe w1-gpio
modprobe w1-therm

# Check if MPU6050 is connected
echo "Checking for MPU6050 sensor..."
i2cdetect -y 1 | grep -q "68"
if [ $? -eq 0 ]; then
    echo "MPU6050 found on I2C bus"
else
    echo "Warning: MPU6050 not detected. Motion sensing will be simulated."
fi

# Configure sensors
echo "Configuring sensors..."
sensors-detect --auto

echo "Sensor setup complete!"
echo "You may need to reboot for all changes to take effect."
