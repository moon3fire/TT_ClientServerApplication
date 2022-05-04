#pragma once
#include <string>

class CMDHandler {
public:
    CMDHandler();
    void Handler(std::string& incomingText, int sockFD = 0);
private:
    void ShowTime(std::string& acceptedCommand);
    void ClientDisconnect(std::string& acceptedCommand, int sockFD = 0);
};

CMDHandler* GetCMDHandler();
