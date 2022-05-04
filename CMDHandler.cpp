#include "CMDHandler.h"
#include <time.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

std::string sc_CMD_STR_GET_TIME = "get_time";
std::string sc_CMD_STR_DISCONNECT = "disconnect";
std::string sc_STR_INVALID = "invalid command";

CMDHandler::CMDHandler()
{

}

void CMDHandler::Handler(std::string& incomingText, int sockFD)
{
    if (incomingText == "get_time")
        return ShowTime(incomingText);
    else if (incomingText == "disconnect")
        return ClientDisconnect(incomingText, sockFD);
    else {
        incomingText = sc_STR_INVALID;  
        return;
    }
}

void CMDHandler::ShowTime(std::string& acceptedCommand)
{
    time_t now = time(NULL);
    struct tm *curTime = localtime(&now);
    char timeBuffer[20];
    memset(timeBuffer , 0 , 20);

    strftime(timeBuffer, 20, "%Y-%m-%d %H:%M:%S", curTime);
    acceptedCommand = std::string(timeBuffer, strlen(timeBuffer));
    return;
}

void CMDHandler::ClientDisconnect(std::string& acceptedCommand, int sockFD)
{
    acceptedCommand = "disconnect accepted, disconnecting you from server...";
	send(sockFD, acceptedCommand.c_str(), strlen(acceptedCommand.c_str()) , 0);
	shutdown(sockFD, SHUT_RDWR);
	close(sockFD);
    return;
}

CMDHandler* GetCMDHandler()
{
    static CMDHandler s_CmdHandler;
    return &s_CmdHandler;
}

