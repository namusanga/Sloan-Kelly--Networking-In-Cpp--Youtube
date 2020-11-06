﻿#include <iostream>
#include <string>
#include "TcpListener.h"

using namespace std;

void Listener_MessageReceived(CTcpListener* listener, int client, string msg);

void main() {
	CTcpListener server("127.0.0.1", 54010, Listener_MessageReceived);

	//if we  are able to ini the server
	if (server.Init()) {
		server.Run();
	}
}

void Listener_MessageReceived(CTcpListener* listener, int client, string msg) {
	listener->Send(client, msg);
}
