#include "ComClient.h"
#include <sstream>
#include <chrono>
#include <iostream>


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
        boost::asio::ip::tcp::no_delay option(true);
        m_socket.set_option(option);

		// Start the io service in a new thread
        startReadHeader();
		m_ioThread = thread(&ComClient::runIoService, this);
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
    cout << "The io service for ComClient ran out of work to do." << endl;
}

/*
void ComClient::SendPing() {

	auto now = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());
    ovdrone::proto::Ping msg;
    msg.set_request_timestamp(now.count());
    msg.set_response_timestamp(now.count());

    SendMessage(&msg, proto::PING);
}
*/

void ComClient::SendMessage(google::protobuf::Message *msg, proto::MessageTypes msgType) {
    // Ensure that only one thread is sending at the same time
    std::lock_guard<std::mutex> lock(m_sendMutex);

    size_t payloadSize = msg->ByteSize();

    m_writeBuffer.resize(HEADER_SIZE + payloadSize);

    // Magic number that is my birthday
    *((uint32_t*)&m_writeBuffer[0]) = htobe32(14041992);
    *((uint32_t*)&m_writeBuffer[4]) = htobe32(payloadSize); // Payload size
    m_writeBuffer[8] = (uint8_t)msgType; // Message type


    if(!msg->SerializeToArray(&m_writeBuffer[HEADER_SIZE], payloadSize)) {
        cout << "[ComClient] Could not serialize message to buffer" << endl;
    }

    try {
        // Write to the socket
        boost::asio::write(m_socket, boost::asio::buffer(m_writeBuffer));
    }
    catch( boost::system::system_error &err ) {
        cout << "[ComClient] Error in SendMessage: " << err.what() << endl;
    }
}


void ComClient::startReadHeader() {
    m_readBuffer.resize(HEADER_SIZE); // Resize to the header size
    boost::asio::async_read(m_socket, boost::asio::buffer(m_readBuffer), std::bind(&ComClient::handleReadHeader, this, std::placeholders::_1));
}

void ComClient::handleReadHeader(const boost::system::error_code &ec) {
    if(!ec) {
        uint32_t magicNumber = be32toh(*(uint32_t*)(&m_readBuffer[0]));
        if(magicNumber != 14041992) {
            cout << "Received invalid magic number" << endl;
            startReadHeader();
            return;
        }
        size_t payloadSize = getPayloadSize();
        //uint8_t msgType = m_readBuffer[8]
        //qDebug() << "Got header";
        startReadBody(payloadSize);
    }
    else {
        cout << "Error in handleReadHeader!" << endl;
        //startReadHeader();
    }
}

size_t ComClient::getPayloadSize() {
    return be32toh(*(uint32_t*)(&m_readBuffer[4]));
}

void ComClient::startReadBody(size_t payloadSize) {
    m_readBuffer.resize(HEADER_SIZE+payloadSize); // Resize to the header size + payload
    boost::asio::async_read(m_socket, boost::asio::buffer(&m_readBuffer[HEADER_SIZE], payloadSize), std::bind(&ComClient::handleReadBody, this, std::placeholders::_1));
}

void ComClient::handleReadBody(const boost::system::error_code &ec) {
    if(!ec) {
        //cout << "Great we received a frame!" << endl;

        uint8_t msgType = m_readBuffer[8];

        switch(msgType) {
        case ovdrone::proto::PING:
            handlePing();
            break;
        default:
            cout << "Unknown message with type " << msgType << " recieved" << endl;
        }

        startReadHeader();
    }
    else {
        cout << "Error in handleReadBody!" << endl;
        startReadHeader();
    }
}

void ComClient::handlePing() {
    auto now = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());
    ovdrone::proto::Ping msg;

    if(!msg.ParseFromArray(&m_readBuffer[HEADER_SIZE], getPayloadSize())) {
        cout << "Could not decode the ping message" << endl;
        return;
    }

    if(!msg.IsInitialized()) {
        cout << "Received an incomplete ping message" << endl;
        return;
    }


    uint64_t timestamp = now.count();
    msg.set_response_timestamp(timestamp);
    SendMessage(&msg, ovdrone::proto::PING);
}
