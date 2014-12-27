#include <iostream>
#include "CameraStreamer.h"
#include "ComClient.h"
#include <string>

using namespace std;
using namespace ovdrone;

int main(int argc, const char** argv) {
	if(argc != 2) {
		cout << "Invalid argument count..." << endl;
		cout << "Usage: tankV1 <controller_hostname>" << endl;
		return 0;
	}

	cout << "Drone started" << endl;
	string controller_host(argv[1]);
	ComClient comClient(controller_host);
	CameraStreamer capt(controller_host);
	capt.Start();
	capt.Join();
    return 0;
}