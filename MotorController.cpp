#include "MotorController.h"

using namespace ovdrone;

MotorController::MotorController() : m_power_left(0.0f), m_power_right(0.0f)
{

}

MotorController::~MotorController()
{
}

std::tuple<float, float> MotorController::get() {
    // Make sure we can't read while we write
    std::lock_guard<std::mutex> lock(m_mutex);
    return std::make_tuple(m_power_left, m_power_right);
}

void MotorController::set(float left, float right) {
    // Make sure we can't read while we write
    std::lock_guard<std::mutex> lock(m_mutex);
    m_last_update = std::chrono::system_clock::now();

    if(left < -1.0f) left = -1.0f;
    else if(left > 1.0f) left = 1.0f;

    if(right < -1.0f) right = -1.0f;
    else if(right > 1.0f) right = 1.0f;

    m_power_left = left;
    m_power_right = right;

    // TODO actually apply power to the motors
}

int MotorController::timeSinceLastUpdate() {
    // Make sure we can't read while we write
    std::lock_guard<std::mutex> lock(m_mutex);
    auto now = std::chrono::system_clock::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>(now-m_last_update).count();
}
