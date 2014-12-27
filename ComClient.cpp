#include "ComClient.h"
#include <sstream>
#include <chrono>
#include <iostream>
#include "tankv1.pb.hpp"

using namespace std;
using namespace ovdrone;
using boost::asio::ip::tcp;

ComClient::ComClient(string target_host) 
: m_remoteHost(target_host), 
	m_ioService(), 
	m_socket(m_ioService)
 {
 	try {
 		tcp::resolver resolver(m_ioService);
		m_endpoint = *resolver.resolve({tcp::v4(), "127.0.0.1", "9123"});

		boost::asio::connect(m_socket, resolver.resolve({tcp::v4(), "127.0.0.1", "9123"}));

		if(!m_socket.is_open()) {
			cout << "[ComClient] Could not connect to target host" << endl;
			return;

		}
		// Start the io service in a new thread
		m_ioThread = thread(&ComClient::runIoService, this);

		SendPing();
 	}
	catch( boost::system::system_error &err ) {
		cout << "[ComClient] Error in connect: " << err.what() << endl;
	}
}

ComClient::~ComClient() {

}

void ComClient::runIoService() {
	cout << "Starting ComClient io service" << endl;
	m_ioService.run();
}

void ComClient::SendPing() {

	auto now = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());
    tankv1::proto::Ping msg;
    msg.set_request_timestamp(now.count());
    msg.set_response_timestamp(now.count());

    size_t payloadSize = msg.ByteSize();

	m_writeBuffer.resize(header_size + payloadSize);

	// Magic number that is my birthday
	*((uint32_t*)&m_writeBuffer[0]) = htobe32(14041992);
	*((uint32_t*)&m_writeBuffer[4]) = htobe32(payloadSize); // Payload size
	m_writeBuffer[8] = tankv1::proto::PING; // Message type


	if(!msg.SerializeToArray(&m_writeBuffer[header_size], payloadSize)) {
		cout << "[ComClient] Could not serialize ping to buffer" << endl;
	}

	try {
		// Write to the socket
		boost::asio::write(m_socket, boost::asio::buffer(m_writeBuffer));
	}
	catch( boost::system::system_error &err ) {
		cout << "[ComClient] Error in SendPing: " << err.what() << endl;
	}
}
