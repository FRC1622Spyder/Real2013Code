//
//#include <stdio.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <netdb.h>
//#include <errno.h>
//#include "Subsystem.h"
//#include "WPIObjMgr.h"
//#include "Config.h"
//#include "Vision/RGBImage.h"
//#include "Vision/BinaryImage.h"
//#include <WPILib.h>
//#include "Math.h"
//#include "vxWorks.h"
//#include <pipeDrv.h>
//
//#define kMaxPacketSize 1536
//#define kImageBufferAllocationIncrement 1000
//
//char buffer[4096];
//char buf[4096];
///**
// * Retrieves image data from the Axis webcam
// * lightweight implimentation of the WPI 
// * AxisCamera class 
// */
//
//
//
//class ImageGrabber : private Target {
//	
//	STATUS namedVxPipe;
//	Task *recvTask;
//	int pipeFd;
//
//	UINT32 m_ipAddress;
//	const char *requestString;
//
//	struct sockaddr_in serverAddr;
//	int sockAddrSize;
//	int camSocket;
//	int m_cameraSocket;
//
//	// super risky shared memory! WOO!
//	bool *readMutex;
//	bool *writeMutex;
//	char *sharedImgBuff;
//
//	int ImageStreamTaskFunction() {
//		// Loop on trying to setup the camera connection. This happens in a background
//		// thread so it shouldn't effect the operation of user programs.
//
//		const char
//				*requestString =
//						"GET /mjpg/video.mjpg HTTP/1.1\n\
//	User-Agent: HTTPStreamClient\n\
//	Connection: Keep-Alive\n\
//	Cache-Control: no-cache\n\
//	Authorization: Basic RlJDOkZSQw==\n\n";
//		//semTake(m_socketPossessionSem, WAIT_FOREVER);
//		m_cameraSocket = CreateCameraSocket(requestString);
//		if (m_cameraSocket == ERROR) {
//			//TODO: write error msg to console and set period to be longer
//			recvTask->Stop();
//		} else {
//			//TODO: set period to normal length
//			//risky much?
//			recvTask->Start();
//			//ReadImagesFromCamera();
//		}
//		return m_cameraSocket;
//	}
//
//	int ReadImagesFromCamera();
//
//	int CreateCameraSocket(const char *requestString) {
//		m_ipAddress = inet_addr("10.16.22.11");
//		/* create socket */
//		if ((camSocket = socket(AF_INET, SOCK_STREAM, 0)) == ERROR) {
//			//wpi_setErrnoErrorWithContext("Failed to create the camera socket");
//			return ERROR;
//		}
//
//		sockAddrSize = sizeof(struct sockaddr_in);
//		bzero((char *) &serverAddr, sockAddrSize);
//		serverAddr.sin_family = AF_INET;
//		serverAddr.sin_len = (u_char) sockAddrSize;
//		serverAddr.sin_port = htons(80);
//
//		serverAddr.sin_addr.s_addr = m_ipAddress;
//
//		/* connect to server */
//		struct timeval connectTimeout;
//		connectTimeout.tv_sec = 5;
//		connectTimeout.tv_usec = 0;
//		if (connectWithTimeout(camSocket, (struct sockaddr *) &serverAddr,
//				sockAddrSize, &connectTimeout) == ERROR) {
//			//wpi_setErrnoErrorWithContext("Failed to connect to the camera");
//			close(camSocket);
//			return ERROR;
//		}
//		int sent = send(camSocket, requestString, strlen(requestString), 0);
//		if (sent == ERROR) {
//			//wpi_setErrnoErrorWithContext("Failed to send a request to the camera");
//			close(camSocket);
//			return ERROR;
//		}
//		return camSocket;
//	}
//
//public:
//	ImageGrabber() {
//		recvTask = new Task(
//				"recvTask",
//				(FUNCPTR) this->ReadImagesFromCamera());
//		pipeDrv();//initialize the pipe driver
//		namedVxPipe=pipeDevCreate("/pipe/image", 60, 1024);//create our device
//		pipeFd = open("/pipe/image", O_RDWR, 0);
//
//	}
//	~ImageGrabber(){
//		close(pipeFd);
//	}
//	RGBImage GetImage();
//	char *GetRawImage();
//
//};
//
//char * ImageGrabber::GetRawImage() { //TODO: check pointers
//	char* buf;
//	read(pipeFd, buf, 1024);
//	return buf;
//	//*writeMutex = true; //take lock
//	//if (&readMutex == false) { //true means lock
//	//	char *buf[sizeof(sharedImgBuff)];
//	//	memcpy(buf, sharedImgBuff, sizeof(sharedImgBuff));
//	//	*writeMutex = false; // release lock
//	//	return *buf;
//	//} else {
//		//other process is busy, try again.
//	//}
//}
//
//int ImageGrabber::ReadImagesFromCamera() {
//	//bool* readMutex = static_cast<bool*> (readMutex_);
//	//bool* writeMutex = static_cast<bool*> (writeMutex_);
//	//char* sharedMem = static_cast<char*> (sharedMem_);
//	char *imgBuffer = NULL;
//	int imgBufferLength = 0;
//	//Infinite loop, task deletion handled by taskDeleteHook
//	// Socket cleanup handled by destructor
//
//	// TODO: these recv calls must be non-blocking. Otherwise if the camera
//	// fails during a read, the code hangs and never retries when the camera comes
//	// back up.
//
//	int counter = 2;
//	
//	int fd = 0;
//	fd = open ("/pipe/image", O_RDWR, 0);
//	while (1) {
//		char initialReadBuffer[kMaxPacketSize] = "";
//		char intermediateBuffer[1];
//		char *trailingPtr = initialReadBuffer;
//		int trailingCounter = 0;
//		while (counter) {
//			// TODO: fix me... this cannot be the most efficient way to approach this, reading one byte at a time.
//			if (recv(m_cameraSocket, intermediateBuffer, 1, 0) == ERROR) {
//				//wpi_setErrnoErrorWithContext("Failed to read image header");
//				close(m_cameraSocket);
//				return ERROR;
//			}
//			strncat(initialReadBuffer, intermediateBuffer, 1);
//			// trailingCounter ensures that we start looking for the 4 byte string after
//			// there is at least 4 bytes total. Kind of obscure.
//			// look for 2 blank lines (\r\n)
//			if (NULL != strstr(trailingPtr, "\r\n\r\n")) {
//				--counter;
//			}
//			if (++trailingCounter >= 4) {
//				trailingPtr++;
//			}
//		}
//		counter = 1;
//		char *contentLength = strstr(initialReadBuffer, "Content-Length: ");
//		if (contentLength == NULL) {
//			//wpi_setWPIErrorWithContext(IncompatibleMode, "No content-length token found in packet");
//			close(m_cameraSocket);
//			return ERROR;
//		}
//		contentLength = contentLength + 16; // skip past "content length"
//		int readLength = atol(contentLength); // get the image byte count
//
//		// Make sure buffer is large enough
//		if (imgBufferLength < readLength) {
//			if (imgBuffer)
//				delete[] imgBuffer;
//			imgBufferLength = readLength + kImageBufferAllocationIncrement;
//			imgBuffer = new char[imgBufferLength];
//			if (imgBuffer == NULL) {
//				imgBufferLength = 0;
//				continue;
//			}
//		}
//
//		// Read the image data for "Content-Length" bytes
//		int bytesRead = 0;
//		int remaining = readLength;
//		while (bytesRead < readLength) {
//			int bytesThisRecv = recv(m_cameraSocket, &imgBuffer[bytesRead],
//					remaining, 0);
//			bytesRead += bytesThisRecv;
//			remaining -= bytesThisRecv;
//		}
//		// Update image
//		//if (&readMutex == false) { //TODO: check pointers 
//		//	*writeMutex = true; //take lock
//		//	memcpy(&sharedMem, &imgBuffer, sizeof(&imgBuffer));
//		//	*writeMutex = false; //release lock
//		//}
//		if(fd != 0) write(fd, imgBuffer, sizeof(imgBuffer));
//		
//		
//	}
//}
//*/
