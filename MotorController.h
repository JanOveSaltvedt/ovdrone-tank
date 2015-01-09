#ifndef MOTORCONTROLLER_H
#define MOTORCONTROLLER_H

#include <thread>
#include <mutex>
#include <utility>
#include <chrono>
#include <tuple>

#ifdef __arm__
#define COMPILE_BEAGLEBONE
#endif

#ifdef COMPILE_BEAGLEBONE
#include "BlackGPIO.h"
#include "BlackPWM.h"
#else
#warning "Beaglebone specific code is not compiled."
#endif

namespace ovdrone {
class MotorController
{
public:
    MotorController();
    ~MotorController();

    void set(float left, float right);
    std::tuple<float, float> get();

    int timeSinceLastUpdate();

private:
    std::mutex m_mutex;
    float m_power_left;
    float m_power_right;
    std::chrono::time_point<std::chrono::system_clock> m_last_update;

#ifdef COMPILE_BEAGLEBONE
    BlackLib::BlackGPIO m_leftMotorEnable;
    BlackLib::BlackGPIO m_rightMotorEnable;
    BlackLib::BlackPWM m_leftForward;
    BlackLib::BlackPWM m_leftBackwards;
    BlackLib::BlackPWM m_rightForward;
    BlackLib::BlackPWM m_rightBackwards;
#endif
};
}
#endif // MOTORCONTROLLER_H
