#include "sensors/camera/camera_capture.hpp"

CameraFrame CameraCapture::captureFrame() {
    return CameraFrame{640, 480, 640 * 480};
}