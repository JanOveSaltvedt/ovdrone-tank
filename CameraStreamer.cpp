#include "CameraStreamer.h"
#include <sstream>
#include <chrono>
#include <thread>

using namespace std;
using namespace ovdrone;
using boost::asio::ip::udp;

CameraStreamer::CameraStreamer(string target_host) 
: m_remoteHost(target_host), 
    m_ioService(),
    m_socket(m_ioService, udp::endpoint(udp::v4(), 0)), m_quality(50), m_frameDelay(30), m_grayscale(false)
 {

	//m_ioService = boost::asio::io_service();
	//m_socket = udp::socket();

    //boost::asio::ip::basic_resolver_query q(udp::v4(), target_host, "daytime");
	udp::resolver resolver(m_ioService);
    m_endpoint = *resolver.resolve({udp::v4(), target_host, "9124"});

}

CameraStreamer::~CameraStreamer() {

}

void CameraStreamer::Restart() {
    m_socket.close();
    m_ioService.reset();

    m_socket = std::move(boost::asio::ip::udp::socket(m_ioService, udp::endpoint(udp::v4(), 0)));

    udp::resolver resolver(m_ioService);
    m_endpoint = *resolver.resolve({udp::v4(), m_remoteHost, "9124"});


}

void CameraStreamer::Start() {
	//Capture();
    m_run = true;
	
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

void CameraStreamer::Stop() {
    m_run = false;
}

void CameraStreamer::Capture() {
	using namespace cv;
    VideoCapture cap(0);

    if(!cap.isOpened()) {
        cout << "Could not open camera for capturing." << endl;
        return;
    }

    cap.set(CV_CAP_PROP_FRAME_WIDTH,320);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT,240);


	Mat frame;
    //Mat receivedFrame;
    Mat grayscaleFrame;
    vector<uchar> buffer;
	vector<int> encodeParams = vector<int>(2);
	encodeParams[0] = CV_IMWRITE_JPEG_QUALITY;
	encodeParams[1] = 20;


    //namedWindow("input");
    //namedWindow("output");
    while(m_run) {
        // Get a timestamp
        auto timestamp = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());
		cap >> frame;

        //imshow("input", frame);

        Mat* pFrame = &frame;
        if(m_grayscale) {
            cvtColor( frame, grayscaleFrame, CV_BGR2GRAY );
            pFrame = &grayscaleFrame;
        }

        encodeParams[1] = m_quality;
        imencode(".jpg", *pFrame, buffer, encodeParams);

		SendFrame(buffer, timestamp.count());
        /*
		imdecode(Mat(buffer), CV_LOAD_IMAGE_COLOR, &receivedFrame);

        stringstream ss;
		ss << "Size: " << buffer.size();
		putText(receivedFrame, ss.str().c_str(), Point2f(10,10), FONT_HERSHEY_PLAIN, 1.0,  Scalar(0,0,255,255));

		ss.str("");
		ss.clear();
		ss << "Timestamp: " << timestamp.count();
		putText(receivedFrame, ss.str().c_str(), Point2f(10,40), FONT_HERSHEY_PLAIN, 1.0,  Scalar(0,0,255,255));
		imshow("output", receivedFrame);

        if(waitKey(10) >= 0) break;*/
        std::this_thread::sleep_for(std::chrono::milliseconds(m_frameDelay));
	}
}

void CameraStreamer::SetFrameDelay(int ms) {
    m_frameDelay = ms;
}

void CameraStreamer::SetQuality(int quality) {
    m_quality = quality;
}

void CameraStreamer::SetGrayscale(bool grayscale) {
    m_grayscale = grayscale;
}
