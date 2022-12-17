#pragma once
#include "TT_Cli.h"
#include "CMDHandler.h"
#include <netinet/in.h>
#include <list>

class Server {
public:
	Server(std::string , std::string);
	void Run();
	static void* CMDHandlingRun(void*);
	bool _ShowTimeToClient(std::vector<std::string>);
	bool _DisconnectClient(std::vector<std::string>);
    	static CMDHandler* CMDHandlerPtr;
private:
	std::list<void*> sockList;
    	std::string m_port;
	std::string m_ipAddress;
	bool m_readyStatus;
	static void* ClientServant(void*);
	int m_listenerSock;
	int m_clientSock;
	int* newSock;
	sockaddr_in m_client;
	sockaddr_in m_server;
};

Server* GetServer(std::string , std::string);
