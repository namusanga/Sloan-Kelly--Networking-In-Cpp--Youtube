#pragma once

#include <string>

#include <WS2tcpip.h>						//Header file for winsock functions
#pragma comment (lib, "ws2_32.lib")			//winsock library file

#define MAX_BUFFER_SIZE (49152)

//Forward declaration of the class
class CTcpListener;

//Callback to data received 
typedef void (*MessageReceivedHandler) (CTcpListener* listener, int socketId, std::string msg);

class CTcpListener {
public:
	CTcpListener(std::string ipAddress, int port, MessageReceivedHandler handler);

	~CTcpListener();

	//send message to specified client
	void Send(int ClientSocket, std::string msg);

	//Initialize winsock
	bool Init();

	//main processing loop
	void Run();

	//Cleanup
	void Cleanup();

private:
	//Create a socket
	SOCKET CreateSocket();

	//Wait for a connection
	SOCKET WaitForConnection(SOCKET listening);

private:
	std::string				m_iPAddress;
	int						m_port;
	MessageReceivedHandler	MessageReceived;

};
