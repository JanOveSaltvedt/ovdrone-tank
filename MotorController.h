#ifndef MOTORCONTROLLER_H
#define MOTORCONTROLLER_H

#include <thread>
#include <mutex>
#include <utility>
#include <chrono>
#include <tuple>

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
};
}
#endif // MOTORCONTROLLER_H
