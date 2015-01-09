#include <iostream>
#include "CameraStreamer.h"
#include "ComClient.h"
#include "MotorController.h"
#include <string>
#include <thread>
#include <chrono>
#include <iomanip>
#include "Utils.h"
#include <csignal>

using namespace std;
using namespace ovdrone;

MotorController* g_pMotorController;

void signalHandler( int signum ) {
    if(g_pMotorController != nullptr) {
        g_pMotorController->set(0.0f, 0.0f);
    }

    cout << "Got signal: " << signum << endl;

    exit(signum);
}

int main(int argc, const char** argv) {
    // register signal SIGINT and signal handler
    signal(SIGINT, signalHandler);

    if(argc != 3) {
		cout << "Invalid argument count..." << endl;
        cout << "Usage: tankV1 <controller_hostname> <wlan_interface_name>" << endl;
		return 0;
	}

	cout << "Drone started" << endl;
	string controller_host(argv[1]);
    string wlan_interface_name(argv[2]);

    MotorController motorController;
    g_pMotorController = &motorController;
    CameraStreamer capt(controller_host);
    ComClient comClient(controller_host, &motorController, &capt);

    Utils utils;
	capt.Start();
    //capt.Join();

    int counter = 0;
    auto wait_until = std::chrono::system_clock::now() + std::chrono::milliseconds(100);
    while(true) {
        std::this_thread::sleep_until(wait_until);
        wait_until = std::chrono::system_clock::now() + std::chrono::milliseconds(100);
        counter++;

        // Below loop should not take more than 100ms in its worst case. However the deadline is soft (for now) so it doesn't matter much...
        tuple<float, float> motorPower = motorController.get();
        if(motorController.timeSinceLastUpdate() > 500) {
           if(std::get<0>(motorPower) != 0.0f || std::get<1>(motorPower) != 0.0f) {
               cout << "Did not receive any motor update for 0.5s. Stopping...";
               motorController.set(0.0f, 0.0f);
           }
        }

        if(counter % 5 == 0) {
            cout << "MotorPower[" << std::fixed << std::setprecision(3) << std::get<0>(motorPower) << ", " << std::get<1>(motorPower) << "]" << endl;
        }

        if(counter % 50 == 0) {
            // Update the network manager
            utils.UpdateWirelessStatus(wlan_interface_name);
            // Send it to the controller...
            comClient.SendNetworkUpdate(utils.getAccessPoint(), utils.getSignalLevel(), utils.getLinkQualityValue(), utils.getLinkQualityMax());
            // Print the output
            cout << "NetworkStatus[AP=" << utils.getAccessPoint() << ", SignalLevel=" << utils.getSignalLevel() << "dBm, LinkQuality=" << utils.getLinkQualityValue() << "/" << utils.getLinkQualityMax() << "]" << endl;
        }
    }
    return 0;
}
