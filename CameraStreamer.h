#ifndef _CAMERA_STREAMER
#define _CAMERA_STREAMER

#include <thread>
#include <string>
#include <opencv2/opencv.hpp>
#include <boost/asio.hpp>


namespace ovdrone {

class CameraStreamer {
public:
	CameraStreamer(std::string target_host);
	~CameraStreamer();

	void Start();
	void Join();

    void SetQuality(int quality);
    void SetGrayscale(bool grayscale);
    void SetFrameDelay(int ms);

private:
	void Capture();
	void SendFrame(std::vector<uchar> &frame, uint64_t timestamp);

private:
	std::string m_remoteHost;
	std::thread m_captureThread;
	boost::asio::io_service m_ioService;
	boost::asio::ip::udp::socket m_socket;
	boost::asio::ip::udp::endpoint m_endpoint;

    int m_quality;
    int m_frameDelay;
    bool m_grayscale;
};
}

#endif
