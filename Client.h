#pragma once
#include <netinet/in.h>
#include <string>
#include "TT_Cli.h"

class Client {
public:
        Client();
        void Run();
		//the functions below describe client application functionality
        bool _GetClientVersion(std::vector<std::string> cmdArgs);
        bool _ConnectWithServer(std::vector<std::string> cmdArgs);
        bool _GetTimeFromServer(std::vector<std::string> cmdArgs);
        bool _DisconnectFromServer(std::vector<std::string> cmdArgs);
private:
        char m_buffer[2000];
        bool m_connectStatus;
        std::string m_currentCommand;
        std::string m_currentIP;
        std::string m_currentPort;
        sockaddr_in m_client;
        int m_sock;
};

//this function will return pointer to static object
Client* GetClient();

