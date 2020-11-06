#include "TcpListener.h"

CTcpListener::CTcpListener(std::string ipAddress, int port, MessageReceivedHandler handler) :
	m_iPAddress(ipAddress), m_port(port), MessageReceived(handler) {

}

CTcpListener::~CTcpListener() {
	Cleanup();
}

//send message to specified client
void CTcpListener::Send(int ClientSocket, std::string msg) {
	send(ClientSocket, msg.c_str(), msg.size() + 1, 0);
}

//Initialize winsock
bool CTcpListener::Init() {
	WSADATA data;
	WORD ver = MAKEWORD(2, 2);

	int wsInit = WSAStartup(ver, &data);

	//TODO:inform caller about the error that occured

	return wsInit == 0;
}


//Wait for a connection
SOCKET CTcpListener::WaitForConnection(SOCKET listening) {
	SOCKET client = accept(listening, NULL, NULL);
	return client;
}

//main processing loop
void CTcpListener::Run() {

	char buf[MAX_BUFFER_SIZE];

	while (true) {
		//Create a listening socket
		SOCKET listening = CreateSocket();
		if (listening == INVALID_SOCKET)
			break;

		SOCKET client = WaitForConnection(listening);
		if (client != INVALID_SOCKET)
		{
			closesocket(listening);

			int bytesReceived = 0;
			do {
				ZeroMemory(buf, MAX_BUFFER_SIZE);
				bytesReceived = recv(client, buf, MAX_BUFFER_SIZE, 0);
				if (bytesReceived > 0) {
					if (MessageReceived != NULL) {
						MessageReceived(this, client, std::string(buf, 0, bytesReceived));
					}
				}
			} while (bytesReceived > 0);

			closesocket(client);
		}

	}
}


//Create a socket
//binds the port and opens a socket for it 
SOCKET CTcpListener::CreateSocket() {
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening != INVALID_SOCKET)
	{
		//make the setup for the socket
		sockaddr_in hint;
		hint.sin_family = AF_INET;
		hint.sin_port = htons(m_port);
		inet_pton(AF_INET, m_iPAddress.c_str(), &hint.sin_addr);

		//bind the socket and ip and port
		int bindok = bind(listening, (sockaddr*)&hint, sizeof(hint));
		if (bindok != SOCKET_ERROR) {
			int listenOk = listen(listening, SOMAXCONN);
			//error
			if (listenOk == SOCKET_ERROR)
				return -1;

			//SUCCESS
		}
		else return -1;
	}

	return listening;
}

void CTcpListener::Cleanup() {
	WSACleanup();
}
