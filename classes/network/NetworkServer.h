#ifndef networkserverH
#define networkserverH

#include "Analyzer.h"
#include "../simpleobject/PhysicWorld2.h"

#define SERVER_VERSION 1

#define CONNECTION_LIMIT 2
#define ST_SERVER_PREPARE 0
#define ST_SERVER_LOBBY 1
#define ST_SERVER_PROCESS 2

struct stClient
{
	SOCKET Socket;
	HANDLE hClientThread;
	stPositionInfo PositionInfo;
	bool Alive;
	bool Ready;
	bool DataRecived;
	bool LevelReady;
	glm::vec2 Velocity;
	float Angle;
	MPhysicObject2 PhysicObject2;
	
	stClient()
	{
		Velocity.x = Velocity.y = 0;
		Angle = 0;
	}
	stClient(SOCKET inSocket, HANDLE inhClientThread)
	{
		Socket = inSocket;
		hClientThread = inhClientThread;
		Alive = true;
		Ready = false;
		DataRecived = false;
		LevelReady = false;
		PositionInfo.Id = 0;
	}
};

struct stClientById
{
	stClient Client;
	stClientById(unsigned char inId)
	{
		Client.PositionInfo.Id = inId;
	}
	bool operator () (stClient inClient)
	{
		return Client.PositionInfo.Id == inClient.PositionInfo.Id;
	}
	bool operator () (stClient* inpClient)
	{
		return Client.PositionInfo.Id == inpClient->PositionInfo.Id;
	}
};

struct stClientLevelReadyCount
{
	stClient Client;
	stClientLevelReadyCount(bool inLevelReady)
	{
		Client.LevelReady = inLevelReady;
	}
	bool operator () (stClient* inpClient)
	{
		return Client.LevelReady == inpClient->LevelReady;
	}
};

struct stClientDataRecivedCount
{
	stClient Client;
	stClientDataRecivedCount(bool inDataRecived)
	{
		Client.DataRecived = inDataRecived;
	}
	bool operator () (stClient* inpClient)
	{
		return Client.DataRecived == inpClient->DataRecived;
	}
};

struct stClientReadyCount
{
	stClient Client;
	stClientReadyCount(bool inReady)
	{
		Client.Ready = inReady;
	}
	bool operator () (stClient* inpClient)
	{
		return Client.Ready == inpClient->Ready;
	}
};

class MNetworkServer;

struct stSocketAddress
{
	SOCKET Socket;
	MNetworkServer* pNetworkServer;
};

class MNetworkServer
{
private:
	HANDLE thAcceptThread;
	HANDLE thProcessThread;
	
	vector<stClient*> Clients;
	vector<stClient*>::iterator it;
	bool TerminateServer;
	int ServerStatus;
	stServerSettings ServerSettings;

	MPhysicWorld2 PhysicWorld2;
	MPhysicObject2 GroundBlock[4];//need to be vector
	vector<stBulletInfo> BulletsInfo;
	map<unsigned char, MPhysicObject2*> Bullets;
	
	void ClearClientsReciveData();
	bool SendAll(const char* Data, int Size);
	void ParseCommand(char* Command);
	bool PrepareAndSendLevelData();
	void KickAll();
	
	friend DWORD WINAPI ProcessThread(void* pParam);
	friend DWORD WINAPI ClientsThread(void* pParam);
	friend DWORD WINAPI AcceptThread(void* pParam);
public:
	MNetworkServer();
	~MNetworkServer();
	void MainCycle();
	void Close();
};

#endif
