#ifndef UTILS_H
#define UTILS_H

#include <string>

namespace ovdrone {

class Utils
{
public:
    Utils();
    ~Utils();

    void UpdateWirelessStatus(std::string &interface_name);

    std::string getAccessPoint();
    int getSignalLevel();
    int getLinkQualityValue();
    int getLinkQualityMax();

private:
    std::string m_accessPoint;
    int m_signalLevel;
    int m_linkQualityVal;
    int m_linkQualityMax;
};

}

#endif // UTILS_H
