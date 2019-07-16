#include "stdafx.h"
#include "classes/network/NetworkServer.h"
#include "classes/network/NetworkClient.h"

int main(int argc, char** argv)
{
    if(argc < 2)
	{
		cout<<"No input parameters."<<endl;
		cout<<"app.exe server - for start server."<<endl;
		cout<<"app.exe client [server_ip] [server_port] - for start client."<<endl;
		return 0;
	}
	if(!strcmp(argv[1], "server"))
	{
		LogFile<<"Starting network server"<<endl;
		MNetworkServer NetworkServer;
		NetworkServer.MainCycle();
		NetworkServer.Close();
	}
	if(!strcmp(argv[1], "client"))
	{
		LogFile<<"Starting network client"<<endl;
		MNetworkClient NetworkClient;
		if(NetworkClient.ConnectToServer(argv[2], argv[3])) NetworkClient.Render();
		NetworkClient.Close();
	}
    return 0;
}
