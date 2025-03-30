#include "sensors/camera/camera_capture.hpp"
#include <random>
#include <ctime>

CameraFrame CameraCapture::captureFrame() {
    // Simulated camera frame
    static std::mt19937 gen(static_cast<unsigned int>(std::time(nullptr)));
    std::uniform_int_distribution<int> width_dist(640, 1920);
    std::uniform_int_distribution<int> height_dist(480, 1080);
    
    CameraFrame frame;
    frame.width = width_dist(gen);
    frame.height = height_dist(gen);
    frame.size = frame.width * frame.height * 3; // RGB format, 3 bytes per pixel
    
    return frame;
}
