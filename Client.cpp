#include "Client.h"
#include <iostream>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define CREATE_USAGE_INFO(CMD_NAME , CMD_ARGS) "usage: " CMD_NAME " " CMD_ARGS "\n"
#define CONCAT_CMD_DEF(CMD_NAME , CMD_DEF) "type " CMD_NAME " to " CMD_DEF
#define COMPLETE_DEF(LINE1 , LINE2) LINE1 LINE2

#define CREATE_CMD_STR(NAME) c_CMD_ ## NAME ## _STR
#define CREATE_CMD_DEF(CMD_DEF) c_CMD_ ## CMD_DEF ## _DEF
#define CREATE_CMD(NAME , VALUE , ARGS , ABOUT) \
	const std::string CREATE_CMD_STR(NAME) = VALUE; \
	const std::string CREATE_CMD_DEF(NAME) = COMPLETE_DEF(CREATE_USAGE_INFO(VALUE , ARGS) , CONCAT_CMD_DEF(VALUE , ABOUT));

static const std::string sc_VERSION = std::to_string(VERSION_CLIENT_MAJOR) + "." +
									  std::to_string(VERSION_CLIENT_MINOR) + "." +
									  std::to_string(VERSION_CLIENT_PATCH);

CREATE_CMD(CONNECT, "connect", "<IP> <PORT>", "connect with the server");
CREATE_CMD(GET_TIME, "get_time", "", "get time from the server");
CREATE_CMD(DISCONNECT, "disconnect", "", "disconnect from the server");
CREATE_CMD(PRINT_VERSION, "get_version", "", "print application version");

//Functionality list what will be given to CLI
CMDListArr CMDList;

bool ConnectWithServer(std::vector<std::string> cmdArgs)
{
	return GetClient()->_ConnectWithServer(cmdArgs);
}

bool GetTimeFromServer(std::vector<std::string> cmdArgs)
{
	return GetClient()->_GetTimeFromServer(cmdArgs);
}

bool DisconnectFromServer(std::vector<std::string> cmdArgs)
{
	return GetClient()->_DisconnectFromServer(cmdArgs);
}

bool GetClientVersion(std::vector<std::string> cmdArgs)
{
	return GetClient()->_GetClientVersion(cmdArgs);
}

Client::Client()
:m_connectStatus(false)
{
	//giving information about our functionality to CLI
	CMDList.insert(CommandDefinition(c_CMD_CONNECT_STR, c_CMD_CONNECT_DEF, &ConnectWithServer));
	CMDList.insert(CommandDefinition(c_CMD_GET_TIME_STR, c_CMD_GET_TIME_DEF, &GetTimeFromServer));
	CMDList.insert(CommandDefinition(c_CMD_DISCONNECT_STR, c_CMD_DISCONNECT_DEF, &DisconnectFromServer));
	CMDList.insert(CommandDefinition(c_CMD_PRINT_VERSION_STR, c_CMD_PRINT_VERSION_DEF, &GetClientVersion));	
    
	memset(m_buffer, 0, BUF_SIZE);
    
    std::cout << "#################################################################################" << std::endl;
    std::cout << "#                             Client Application Version " << sc_VERSION << "                  #" << std::endl;
    std::cout << "#################################################################################" << std::endl;

    std::cout << " Type help for additional information" << std::endl;
    std::cout << " Default server IP 127.0.0.1 , port 7821" << std::endl;
}

void Client::Run()
{
    TT_Cli* CLI = GetCLI();
	CLI->RegisterCMDList(CMDList);
	while (CLI->GetLine()) {
		continue;
	}
}

Client* GetClient()
{
    static Client s_client;
    return &s_client;
}

bool Client::_ConnectWithServer(std::vector<std::string> cmdArgs)
{
	bool retVal = true;
	
	//case where the user is already connected
	
	if (m_connectStatus) {
		std::cout << "You are already connected to server" << std::endl;
		std::cout << "IP:" << m_currentIP << "  |   Port:" << m_currentPort << std::endl;
		return retVal;
	}
	if (cmdArgs.size() < 3) {
		std::cout << "Not enough arguments | Use connect <IP> <PORT>" << std::endl;
		return retVal;
	}
	else if (cmdArgs.size() > 3) {
		std::cout << "Too much arguments | Use connect <IP> <PORT>" << std::endl;
		return retVal;
	}
	else if (cmdArgs[1].size() < 7 || std::atoi(cmdArgs[2].c_str()) < 0) {
		std::cout << "Not correct IP address or port" << std::endl;
		return retVal;
	}

	//cmdArgs first element is command name, second is ip address given to us by user and third is port number
	
	m_currentIP = cmdArgs[1];
	m_currentPort = cmdArgs[2];
	m_sock = socket(AF_INET, SOCK_STREAM, 0);
	
	//in case socket wasn't created

	if (m_sock == -1) {
		std::cerr << "Couldn't create socket\n";
		retVal = false;
		return retVal;
	}

	//choosing ipv4 type, initializing it with user given ip address and port number
	m_client.sin_family = AF_INET;
	m_client.sin_port = htons(std::atoi(cmdArgs[2].c_str()));
	m_client.sin_addr.s_addr = inet_addr(cmdArgs[1].c_str());

	//trying to connect with given ip address at given port number
	int connectResult = connect(m_sock, (sockaddr*)&m_client, sizeof(m_client));
	if (connectResult == -1) {
		std::cerr << "Connection Fault\n";
		return retVal;
	}

	std::cout << "Connected with the server" << std::endl;
	// Here we can add some recieved information in case if server will send as something like greeting
	m_connectStatus = true;
	return retVal;
}

bool Client::_GetTimeFromServer(std::vector<std::string> cmdArgs)
{
	if (m_connectStatus == false) {
		std::cerr << "You are not connected with server\n";
		return true;
	}
	send(m_sock, cmdArgs[0].c_str(), cmdArgs[0].size(), 0);
	memset(m_buffer, 0, BUF_SIZE);
	int bytesRecieved = recv(m_sock, m_buffer, BUF_SIZE, 0);
	if (bytesRecieved < 0) {
		std::cerr << "Connection Lost , disconnecting from server...\n";
		return true;
	}
	std::cout << "Server: " << std::string(m_buffer, bytesRecieved) << std::endl;
	return true;
}

bool Client::_DisconnectFromServer(std::vector<std::string> cmdArgs)
{
	if (m_connectStatus == false) {
		std::cerr << "You are not connected with server\n";
		return true;
	}
	std::string str = "disconnect";
	send(m_sock, cmdArgs[0].c_str(), cmdArgs[0].size(), 0);
	int bytesRecieved = recv(m_sock, m_buffer, 2000, 0);
	if (bytesRecieved < 0) {
		std::cerr << "Connection Lost , disconnecting from server\n";
	}
	std::cout << std::string(m_buffer, bytesRecieved) << std::endl;
	std::cout << "disconnecting from server..." << std::endl;
	close(m_sock);
	m_connectStatus = false;
	return true;
}

bool Client::_GetClientVersion(std::vector<std::string> cmdArgs)
{
    std::cout << " Client Application Version " << sc_VERSION << std::endl;
	GetCLI()->_GetCLIVersion(cmdArgs);
	return true;
}

