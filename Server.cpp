#include "Server.h"
#include "CMDHandler.h"
#include "TT_Cli.h"
#include <sys/socket.h>
#include <time.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <algorithm>
#include <arpa/inet.h>

CMDListArr CMDList;
int clientID = 0;

Server* GetServer(std::string ip, std::string port)
{
	static Server s_server(ip, port);
	return &s_server;
}


Server::Server(std::string ip, std::string port)
:m_readyStatus(false)
,m_ipAddress(ip)
,m_port(port)
{
	m_listenerSock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_listenerSock == -1) {
		std::cout << "Couldn't create socket" << std::endl;
		return;
	}

	m_server.sin_family = AF_INET;
//	m_server.sin_addr.s_addr = INADDR_ANY;
	m_server.sin_port = htons(std::atoi(m_port.c_str()));
	m_server.sin_addr.s_addr = inet_addr(ip.c_str());
	inet_pton(AF_INET , ip.c_str() , &m_server.sin_addr);
	
	int optional = 1;
	setsockopt(m_listenerSock , SOL_SOCKET , SO_REUSEADDR , &optional , sizeof(optional));

	int bindResult = bind(m_listenerSock , (sockaddr*)&m_server , sizeof(m_server));
	if (bindResult == -1) {
		shutdown(m_listenerSock , SHUT_RDWR);
		close(m_listenerSock);
		std::cout << "Bind phase failed" << std::endl;
		return;
	}

	int listenResult = listen(m_listenerSock, 5);
	if (listenResult == -1) {
		shutdown(m_listenerSock, SHUT_RDWR);
		close(m_listenerSock);
		std::cout << "Listening phase failed" << std::endl;
		return;
	}
	m_readyStatus = true;
}

void* Server::ClientServant(void* socket)
{
	int currentClientID = clientID;
	int sock = *(int*)(socket);
	char buffer[2000];
	CMDHandler* cmdPtr = GetCMDHandler();
	while (sock > 0) {
		memset(buffer , 0 , 2000);
		
		int bytesRecieved = recv(sock , buffer , 2000 , 0);
		if (bytesRecieved < 0) {
			free(socket);
			return 0;
		}
        std::string str = std::string(buffer , bytesRecieved);
        cmdPtr->Handler(str , sock);
        send(sock, str.c_str(), str.size(), 0);
	}
	shutdown(sock, SHUT_RDWR);
	close(sock);
	free(socket);
	return 0;
}

void Server::Run()
{

	if (!m_readyStatus) {
		std::cout << "Server has been established uncorrectly" << std::endl;
		return;
	}

	std::cout << "Server starting work..." << std::endl;
    int size = sizeof(m_client);
	while (m_clientSock = accept(m_listenerSock, (struct sockaddr*)&m_client, (socklen_t*)&size)) {
		++clientID;
	
		pthread_t userThread;
		newSock = (int*)malloc(1);
		*newSock = m_clientSock;
		sockList.emplace_back((void*)newSock);
		int threadCreateResult = pthread_create(&userThread , NULL , ClientServant, (void*)newSock);
		if (threadCreateResult < 0) {
			std::cout << "Thread creating phase failed" << std::endl;
			break;
		}
	}

	shutdown(m_listenerSock, SHUT_RDWR);
	shutdown(m_clientSock , SHUT_RDWR);
	close(m_listenerSock);
	close(m_clientSock);
}

