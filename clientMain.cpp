#include "Client.h"

int main()
{
    Client* client;
	//Getting static object of class Client and running it
    
	client = GetClient();
    client->Run();
}
