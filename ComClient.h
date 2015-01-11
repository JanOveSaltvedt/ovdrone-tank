#ifndef _COM_CLIENT
#define _COM_CLIENT

#include <thread>
#include <string>
#include <opencv2/opencv.hpp>
#include <boost/asio.hpp>
#include "ovdrone.pb.h"
#include <mutex>
#include "MotorController.h"
#include "CameraStreamer.h"

namespace ovdrone {

class ComClient {
public:
    ComClient(std::string target_host, MotorController *mc, CameraStreamer *cs);
	~ComClient();

    void SendMessage(google::protobuf::Message* msg, ovdrone::proto::MessageTypes msgType);

    void SendNetworkUpdate(const std::string &ap, int signalLevel, int linkQualityVal, int linkQualityMax);

    void Stop();

private:
	void runIoService();

    void startReadHeader();
    void handleReadHeader(const boost::system::error_code &ec);
    size_t getPayloadSize();

    void startReadBody(size_t payloadSize);
    void handleReadBody(const boost::system::error_code &ec);

    void handlePing();
    void handleMotorUpdate();
    void handleVideoSettings();

private:
    bool m_run;
	std::string m_remoteHost;
	std::thread m_ioThread;
	boost::asio::io_service m_ioService;
	boost::asio::ip::tcp::socket m_socket;
	boost::asio::ip::tcp::endpoint m_endpoint;
    std::mutex m_sendMutex;

    enum { HEADER_SIZE = 9 };
	std::vector<uint8_t> m_readBuffer;
	std::vector<uint8_t> m_writeBuffer;

    MotorController *m_motorController;
    CameraStreamer *m_cameraStreamer;
};
}

#endif
