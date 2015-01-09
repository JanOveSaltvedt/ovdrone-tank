#include "MotorController.h"

using namespace ovdrone;

/*
PIN_ENA = "P9_23" =>
PIN_ENB = "P9_12"
PIN_MOTOR_LEFT_FW = "P9_21"
PIN_MOTOR_LEFT_BK = "P9_22"
PIN_MOTOR_RIGHT_BK = "P9_16"
PIN_MOTOR_RIGHT_FW = "P9_14"
*/


MotorController::MotorController() : m_power_left(0.0f), m_power_right(0.0f)
#ifdef COMPILE_BEAGLEBONE
  ,
    m_leftMotorEnable(BlackLib::GPIO_49, BlackLib::output, BlackLib::FastMode),
    m_rightMotorEnable(BlackLib::GPIO_60, BlackLib::output, BlackLib::FastMode),
    m_leftForward(BlackLib::P9_21),
    m_leftBackwards(BlackLib::P9_22),
    m_rightForward(BlackLib::P9_14),
    m_rightBackwards(BlackLib::P9_16)
#endif
{

#ifdef COMPILE_BEAGLEBONE
    m_leftForward.setPeriodTime(100000);
    m_leftBackwards.setPeriodTime(100000);
    m_rightBackwards.setPeriodTime(100000);
    m_rightForward.setPeriodTime(100000);
#endif
}

MotorController::~MotorController()
{
#ifdef COMPILE_BEAGLEBONE

#endif
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

    // Actually apply power to the motors
#ifdef COMPILE_BEAGLEBONE
    if(left != 0.0f || right != 0.0f) {
        m_leftMotorEnable.setValue(BlackLib::high);
        m_rightMotorEnable.setValue(BlackLib::high);
    }
    else {
        m_leftMotorEnable.setValue(BlackLib::low);
        m_rightMotorEnable.setValue(BlackLib::low);
    }

    if(left >= 0.0f) {
        m_leftBackwards.setDutyPercent(0.0f);
        m_leftForward.setDutyPercent(left*100.0f);
    }
    else {
        m_leftBackwards.setDutyPercent(left*-100.0f);
        m_leftForward.setDutyPercent(0);
    }

    if(right >= 0.0f) {
        m_rightBackwards.setDutyPercent(0.0f);
        m_rightForward.setDutyPercent(left*100.0f);
    }
    else {
        m_rightBackwards.setDutyPercent(left*-100.0f);
        m_rightForward.setDutyPercent(0);
    }

#endif
}

int MotorController::timeSinceLastUpdate() {
    // Make sure we can't read while we write
    std::lock_guard<std::mutex> lock(m_mutex);
    auto now = std::chrono::system_clock::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>(now-m_last_update).count();
}
