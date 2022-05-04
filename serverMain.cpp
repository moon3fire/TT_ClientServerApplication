#include "Server.h"
#include <cstdlib>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <signal.h>
#include <string>
#include <iostream>

std::string ipAddr = "127.0.0.1";
std::string portNum = "7821";

bool getOptionsArgs(int argc, char* argv[]);

int main(int argc, char* argv[])
{
	getOptionsArgs(argc, argv);
	std::cout << "Server IP: " << ipAddr << " | " << "Port: " << portNum;

	pid_t pid;
	int x_fd;
	pid = fork();
	if (pid < 0) {
		printf("Fork Failed");
		exit(EXIT_FAILURE);
	}
	else if (pid > 0) {
		exit(EXIT_SUCCESS);
	}
	
	//starting new session
	if (setsid() < 0) {
		exit(EXIT_FAILURE);
	}

	signal(SIGCHLD, SIG_IGN);
	signal(SIGHUP, SIG_IGN);

	pid = fork();

	if (pid < 0) {
		exit(EXIT_FAILURE);
	}
	else if (pid > 0) {

	}

	umask(0);

	chdir("/");

	for (x_fd = sysconf(_SC_OPEN_MAX); x_fd >= 0; x_fd--) {
		close(x_fd);
	}

	//starting server from daemon process
	GetServer(ipAddr, portNum)->Run();
}

bool getOptionsArgs(int argc, char* argv[])
{
	bool retVal = true;
	int option;
	while ((option = getopt(argc , argv , "i:p:")) != -1) {
		switch (option) {
			case 'i':
				ipAddr = std::string(optarg , strlen(optarg));
				break;
			case 'p':
				portNum = std::string(optarg , strlen(optarg));
				break;
			default:
				break;
		}	
	}
	return retVal;	
}

