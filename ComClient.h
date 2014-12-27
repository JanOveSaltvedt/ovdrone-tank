#ifndef _COM_CLIENT
#define _COM_CLIENT

#include <thread>
#include <string>
#include <opencv2/opencv.hpp>
#include <boost/asio.hpp>


namespace ovdrone {

class ComClient {
public:
	ComClient(std::string target_host);
	~ComClient();

	void SendPing();

private:
	void runIoService();

private:
	std::string m_remoteHost;
	std::thread m_ioThread;
	boost::asio::io_service m_ioService;
	boost::asio::ip::tcp::socket m_socket;
	boost::asio::ip::tcp::endpoint m_endpoint;

	enum { header_size = 9 };
	std::vector<uint8_t> m_readBuffer;
	std::vector<uint8_t> m_writeBuffer;
};
}

#endif