#include <iostream>
#include <WS2tcpip.h>
#include <sstream>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void main() {

	//initialize winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0) {
		cerr << "Can't init winsock" << endl;
		return;
	}

	//create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "Can't create a socket, quitting" << endl;
		return;
	}
	//bind an ip address and port to the socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(listening, (sockaddr*)&hint, sizeof(hint));

	//tell winsock the socket is for listening
	listen(listening, SOMAXCONN);

	fd_set master;
	FD_ZERO(&master);

	//add listening socket
	FD_SET(listening, &master);

	while (true) {
		//create a copy of the master fd set
		fd_set copy = master;

		//check if any fd set has received a message
		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		//go through all sockets that have received messages
		for (int i = 0; i < socketCount; i++)
		{
			SOCKET sock = copy.fd_array[i];
			if (sock == listening) {
				//ACCEPT THE INCOMING CONNECTION
				SOCKET client = accept(listening, nullptr, nullptr);
				//Add the new connection to the list of connected clients
				FD_SET(client, &master);
				//Send welcome message to the connected client 
				string welcomeMsg = "Welcome to the awesome chat server!\r\n";
				send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);

			}
			else {
				//ACCEPT A NEW MESSAGE
				char buf[4096];
				ZeroMemory(buf, 4096);
				//Receive message
				int bytesIn = recv(sock, buf, 4096, 0);
				if (bytesIn <= 0) {
					//drop the client
					closesocket(sock);
					FD_CLR(sock, &master);
				}
				else {
					//send the message to the other clients on the network
					for (int i = 0; i < master.fd_count; i++)
					{
						SOCKET outSock = master.fd_array[i];
						if (outSock != listening && outSock != sock) {

							ostringstream ss;
							ss << "SOCKET #" << sock << ":" << buf << "\r\n";
							string strOut = ss.str();
							send(outSock, strOut.c_str(), strOut.size() + 1, 0);
						}
					}
				}
			}
		}
	}


	//shutdown winsock
	WSACleanup();

}
////wait for a connection
//sockaddr_in client;
////close listening socket
//closesocket(listening);

////while loop: accept and echo message back to client
//char buf[4096];
//while (true) {
//	ZeroMemory(buf, 4096);

//	//wait for client to send data
//	int bytesReceived = recv(clientSocket, buf, 4096, 0);
//	if (bytesReceived == SOCKET_ERROR) {
//		cerr << "Error in recv().  Quitting" << endl;
//		break;
//	}

//	if (bytesReceived == 0) {
//		cout << "client disconnected" << endl;
//		break;
//	}

//	cout << string(buf, 0, bytesReceived) << endl;

//	//echo message back to client
//	send(clientSocket, buf, bytesReceived + 1, 0);

//}
//int clientSize = sizeof(client);

//SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

//char host[NI_MAXHOST]; //client's remote name
//char service[NI_MAXHOST]; //service (i.e. port) the client is connected on 

//ZeroMemory(host, NI_MAXHOST); //same as memset(host, 0, NI_MAXHOST)
//ZeroMemory(service, NI_MAXHOST);

//if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
//	cout << host << " connected on port " << service << endl;
//}
//else {
//	inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
//	cout << " host connected on port " << ntohs(client.sin_port) << endl;
//}
