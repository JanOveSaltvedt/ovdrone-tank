#include "CameraStreamer.h"
#include <sstream>
#include <chrono>

using namespace std;
using namespace ovdrone;
using boost::asio::ip::udp;

CameraStreamer::CameraStreamer(string target_host) 
: m_remoteHost(target_host), 
	m_ioService(), 
	m_socket(m_ioService, udp::endpoint(udp::v4(), 0))
 {

	//m_ioService = boost::asio::io_service();
	//m_socket = udp::socket();

	udp::resolver resolver(m_ioService);
	m_endpoint = *resolver.resolve({udp::v4(), "127.0.0.1", "9124"});

}

CameraStreamer::~CameraStreamer() {

}

void CameraStreamer::Start() {
	//Capture();
	
	m_captureThread = thread(&CameraStreamer::Capture, this);
}

void CameraStreamer::Join() {
	m_captureThread.join();
}

void CameraStreamer::SendFrame(vector<uchar> &frame, uint64_t timestamp) {
	// We must split the frame up into several packets (most likely)
	

	const uint32_t header_size = 8 + 4 + 4 + 4; // Timestamp + size + offset + length
	// Create a buffer that holds the header
	uint8_t header_buffer[header_size]; 
	*((uint64_t*)&header_buffer[0]) = htobe64(timestamp);
	uint32_t size = frame.size();
	*((uint32_t*)&header_buffer[8]) = htobe32(size);


	
	uint32_t written = 0;
	while(written < size) {
		
		const uint32_t max_payload_size = 512 - header_size; // 512 - header (which is)
		uint32_t left = size - written;
		uint32_t payload_size = min(max_payload_size, left);

		// Write the rest of the header
		*((uint32_t*)&header_buffer[12]) = htobe32(written);
		*((uint32_t*)&header_buffer[16]) = htobe32(payload_size);

		// buffers to send
		vector<boost::asio::const_buffer> buffers;
		buffers.push_back(boost::asio::buffer(header_buffer));
		buffers.push_back(boost::asio::buffer(&frame[written], payload_size));
		m_socket.send_to(buffers, m_endpoint);

		written += payload_size;		
	}

}


void CameraStreamer::Capture() {
	using namespace cv;
	VideoCapture cap(0);
	Mat frame;
	Mat receivedFrame;
	vector<uchar> buffer;
	vector<int> encodeParams = vector<int>(2);
	encodeParams[0] = CV_IMWRITE_JPEG_QUALITY;
	encodeParams[1] = 20;


	namedWindow("input");
	namedWindow("output");
	while(true) {
		cap >> frame;
		// Get a timestamp
		auto timestamp = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());

		imshow("input", frame);

		imencode(".jpg", frame, buffer, encodeParams);

		SendFrame(buffer, timestamp.count());
		
		imdecode(Mat(buffer), CV_LOAD_IMAGE_COLOR, &receivedFrame);

		stringstream ss;
		ss << "Size: " << buffer.size();
		putText(receivedFrame, ss.str().c_str(), Point2f(10,10), FONT_HERSHEY_PLAIN, 1.0,  Scalar(0,0,255,255));

		ss.str("");
		ss.clear();
		ss << "Timestamp: " << timestamp.count();
		putText(receivedFrame, ss.str().c_str(), Point2f(10,40), FONT_HERSHEY_PLAIN, 1.0,  Scalar(0,0,255,255));
		imshow("output", receivedFrame);

		if(waitKey(10) >= 0) break;
	}
}