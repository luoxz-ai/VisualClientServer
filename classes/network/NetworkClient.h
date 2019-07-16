#ifndef networkclientH
#define networkclientH

#define CLIENT_VERSION 1

#include "Analyzer.h"
#include "../system/GraphicClient.h"

using namespace std;

class MNetworkClient: public MGraphicClient
{
private:
	unsigned char Id;
	SOCKET Server;
	
	HANDLE thServerThread;
	DWORD dwServerThread;

	bool TerminateServer;
	unsigned int ServerStatus;
	
	stPositionInfo MainPosition;
public:
	MNetworkClient();
	~MNetworkClient();
	bool Prepare();
	bool AnalyzePacket(stPacket* inpPacket);
	friend DWORD WINAPI ServerThread(void* pParam);
	bool ConnectToServer(const char* incServerName, const char* incServerPort);
	bool GetTerminateServer();
	int GetServerStatus();
	void Close();
	void OnMouseClick(glm::vec2 Position);
};

#endif
