#include "Utils.h"

#include <string>
#include <iostream>
#include <stdio.h>
#include <regex>

using namespace ovdrone;


Utils::Utils()
{

}

Utils::~Utils()
{

}




std::string exec(std::string &cmd) {
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while(!feof(pipe)) {
        if(fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);
    return result;
}

void Utils::UpdateWirelessStatus(std::string &interface_name) {
    std::string cmd = "iwconfig ";
    cmd += interface_name;
    std::string iwOutput = exec(cmd);

    std::smatch m;

    // Search for Access Point
    std::regex e ("Access Point: ([0-9A-Z]{2}:[0-9A-Z]{2}:[0-9A-Z]{2}:[0-9A-Z]{2}:[0-9A-Z]{2}:[0-9A-Z]{2})", std::regex_constants::ECMAScript | std::regex_constants::icase);
    if(std::regex_search (iwOutput,m,e)) {
        // Found access point
        m_accessPoint = m[1];
    }
    else {
        m_accessPoint = "??:??:??:??:??:??";
    }

    // Search for singal level
    e = std::regex("Signal Level=(-?[0-9]+) dBm", std::regex_constants::ECMAScript | std::regex_constants::icase);
    if(std::regex_search (iwOutput,m,e)) {
        // Found signal level
        m_signalLevel = std::stoi(m[1]);
    }
    else {
        m_signalLevel = -99;
    }

    // Search for link quality
    e = std::regex("Link Quality=([0-9]+)/([0-9]+)", std::regex_constants::ECMAScript | std::regex_constants::icase);
    if(std::regex_search (iwOutput,m,e)) {
        // Found link quality
        m_linkQualityVal = std::stoi(m[1]);
        m_linkQualityMax = std::stoi(m[2]);
    }
    else {
        m_linkQualityVal = 0;
        m_linkQualityMax = 99;
    }
}


std::string Utils::getAccessPoint() {
    return m_accessPoint;
}

int Utils::getSignalLevel() {
    return m_signalLevel;
}

int Utils::getLinkQualityValue() {
    return m_linkQualityVal;
}

int Utils::getLinkQualityMax() {
    return m_linkQualityMax;
}
