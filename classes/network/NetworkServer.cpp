#include "../../stdafx.h"
#include "NetworkServer.h"

MNetworkServer::MNetworkServer()
{
	thAcceptThread = 0;
	thProcessThread = 0;
	TerminateServer = true;
	ServerStatus = ST_SERVER_PREPARE;
	ServerSettings.Version = SERVER_VERSION;
	ServerSettings.ClientsLimit = CONNECTION_LIMIT;
	ServerSettings.PlayerSettings.HalfSize = glm::vec2(0.64, 0.64);
	ServerSettings.PlayerSettings.Health = 100;
	ServerSettings.PlayerSettings.Velocity = glm::vec2(0.02, 0.02);
	ServerSettings.BulletSettings.HalfSize = glm::vec2(0.08, 0.08);
	ServerSettings.BulletSettings.Damage = 5;
	ServerSettings.BulletSettings.Velocity = glm::vec2(0.04, 0.04);
}

MNetworkServer::~MNetworkServer()
{
	thAcceptThread = 0;
	thProcessThread = 0;
	TerminateServer = true;
	ServerStatus = ST_SERVER_PREPARE;
}

void MNetworkServer::ClearClientsReciveData()
{
	for(int i=0; i<Clients.size(); i++)
		Clients[i]->DataRecived = false;
}

bool MNetworkServer::SendAll(const char* Data, int Size)
{
	int ResultTotal = 0;
	int Result;
	for(int i=0; i<Clients.size(); i++)
	{
		Result = send(Clients[i]->Socket, Data, Size, 0 );
		if(Result == Size) ResultTotal ++;
	}
	return Clients.size() == ResultTotal;
}

DWORD WINAPI ProcessThread(void* pParam)
{
	MNetworkServer* pNetworkServer = (MNetworkServer*)pParam;
	if(!pNetworkServer)
	{
		cout<<"Can not start process thread"<<endl;
		return 0;
	}
	cout<<"Start process thread"<<endl;

	int Result;
	int Size;
	char temp[HEADER_SIZE + BODY_SIZE];
	MTLVpacket TLVpacket;
	vector<stPositionInfo> Positions;
	while(!pNetworkServer->TerminateServer)
	{
		if(pNetworkServer->Clients.size() && 
			pNetworkServer->ServerStatus == ST_SERVER_PROCESS &&
			count_if(pNetworkServer->Clients.begin(), pNetworkServer->Clients.end(), stClientDataRecivedCount(true)) == pNetworkServer->Clients.size())
		{
			memset(temp, 0, HEADER_SIZE + BODY_SIZE);
			Positions.clear();
			//set velocity
			for(int i=0; i<pNetworkServer->Clients.size(); i++)
			{
				pNetworkServer->Clients[i]->PhysicObject2.SetVelocity(pNetworkServer->Clients[i]->Velocity);
			}
			//world step
			pNetworkServer->PhysicWorld2.Step();
			//prepare for send
			for(int i=0; i<pNetworkServer->Clients.size(); i++)
			{
				pNetworkServer->Clients[i]->PositionInfo.Position = GetQuadCenter(pNetworkServer->Clients[i]->PhysicObject2.GetCollisionBox());
				pNetworkServer->Clients[i]->PositionInfo.RotationAngle = pNetworkServer->Clients[i]->Angle;
				Positions.push_back(pNetworkServer->Clients[i]->PositionInfo);
			}
			pNetworkServer->ClearClientsReciveData();
			//send positions
			TLVpacket.Create0x21(temp, Size, &Positions);
			if(!pNetworkServer->SendAll(temp, Size)) cout<<"Send 0x21 to all failed"<<endl;
			//send bullets info
			if(pNetworkServer->BulletsInfo.size())
			{
				TLVpacket.Create0x51(temp, Size, &pNetworkServer->BulletsInfo);
				if(!pNetworkServer->SendAll(temp, Size)) cout<<"Send 0x51 to all failed"<<endl;
			}
		}
		Sleep(1);
	}
	return 1;
}

DWORD WINAPI ClientsThread(void* pParam)
{
	cout<<"Trying connect new client"<<endl<<"> ";
	stSocketAddress* pSocketAddress = (stSocketAddress*)pParam;
	if(!pSocketAddress)
	{
		cout<<"Null pSocketAddress"<<endl<<"> ";
		return 0;
	}
	
	unsigned char RandId;
	SOCKET ClientSocket = pSocketAddress->Socket;
	MNetworkServer* pNetworkServer = pSocketAddress->pNetworkServer;
	if(!pNetworkServer)
	{
		cout<<"Null client socket"<<endl<<"> ";
		return 0;
	}

	stClient Client(ClientSocket, GetCurrentThread());
	RandId = rand() % 255 + 1;
	pNetworkServer->it = find_if(pNetworkServer->Clients.begin(), pNetworkServer->Clients.end(), stClientById(RandId));
	while(pNetworkServer->it != pNetworkServer->Clients.end())
	{
		RandId = rand() % 255 + 1;
		pNetworkServer->it = find_if(pNetworkServer->Clients.begin(), pNetworkServer->Clients.end(), stClientById(RandId));
	}
	Client.PositionInfo.Id = RandId;				
	pNetworkServer->Clients.push_back(&Client);
	
	int Result;
	int Size;
	char temp[HEADER_SIZE + BODY_SIZE];
	unsigned char EhloStatus = EHLO_UNKNOWN;
	MAnalyzer Analyzer;
	MTLVpacket TLVpacket;

	char* cAddress = NULL;
	sockaddr_in From;
	int Length = sizeof(sockaddr_in);
	if(getsockname(Client.Socket, (struct sockaddr *)&From, &Length) == -1) cout<<"Can not get sockname"<<endl<<"> ";
	else cAddress = inet_ntoa(From.sin_addr);
	cout<<"Clnt con: th:"<<GetCurrentThreadId()<<" adr: "<<cAddress<<endl<<"> ";
	
	glm::vec2 Velocity;
	float Angle;
	
	stBulletInfo AddBulletInfo;
	vector<stBulletInfo>::iterator itBullets;
	unsigned char BulletId;
	MPhysicObject2* PhysicObject2;
	
	while(Client.Alive && !pNetworkServer->TerminateServer)
	{
		memset(temp, 0, HEADER_SIZE + BODY_SIZE);
		Result = recv(Client.Socket, temp, HEADER_SIZE + BODY_SIZE, 0);
		if(Result <= 0)
		{
			cout<<"Client "<<cAddress<<"("<<(int)Client.Socket<<"): "<<" disconnected"<<endl<<"> ";
			Client.Alive = false;
			closesocket(Client.Socket);
			pNetworkServer->PhysicWorld2.RemoveDynamicObjectForce(&Client.PhysicObject2);//dungerous
			pNetworkServer->Clients.erase(find(pNetworkServer->Clients.begin(), pNetworkServer->Clients.end(), &Client));//dungerous
			//send to all info about disconnected client
			TLVpacket.Create0x31(temp, Size, Client.PositionInfo.Id);
			if(!pNetworkServer->SendAll(temp, Size)) cout<<"0x10: Send failed"<<endl;
			return 0;
		}
		Analyzer.FillPackets(temp, Result);
		for(int i=0; i<Analyzer.Packets.size(); i++)
		{
			switch(Analyzer.Packets[i].Type)
			{
				case 0x10://hello
					cout<<"Helo: "<<(int)Analyzer.Packets[i].Type<<" "<<(int)Analyzer.Packets[i].Size<<" "<<(int)Analyzer.Packets[i].Body[0]<<endl<<"> ";
					EhloStatus = EHLO_OK;
					if(Analyzer.Packets[i].Body[0] != SERVER_VERSION) EhloStatus = EHLO_WRONG_VERSION;
					if(pNetworkServer->Clients.size() > CONNECTION_LIMIT) EhloStatus = EHLO_SERVER_FULL;
					if(EhloStatus != EHLO_OK) TLVpacket.Create0x11(temp, Size, EhloStatus, 0, NULL);
					else TLVpacket.Create0x11(temp, Size, EhloStatus, Client.PositionInfo.Id, &pNetworkServer->ServerSettings);
					Result = send(Client.Socket, temp, Size, 0);
					if(Result != Size) cout<<"0x10: Send failed"<<endl<<"> ";
					break;
								
				case 0x12://ready/unready to start
					if(EhloStatus != EHLO_OK) break;
					cout<<"From: ("<<(int)Client.Socket<<"): ";
					cout<<"Status: "<<(int)Analyzer.Packets[i].Body[0]<<endl<<"> ";
					Client.Ready = Analyzer.Packets[i].Body[0];
					break;
				
				case 0x23://level data confirmation
					if(EhloStatus != EHLO_OK) break;
					Client.LevelReady = true;
					break;
								
				case 0x20://recive confirmation and personal data
					if(EhloStatus != EHLO_OK) break;
					memcpy((void*)&Client.Velocity, (void*)Analyzer.Packets[i].Body, VEC2SIZE);
					memcpy((void*)&Client.Angle, (void*)(Analyzer.Packets[i].Body + VEC2SIZE), POINTSIZE);
					Client.DataRecived = true;
					break;
					
				case 0x50://new bullet
					cout<<"0x50"<<endl;
					if(EhloStatus != EHLO_OK) break;
					memcpy((void*)&AddBulletInfo.Position, Analyzer.Packets[i].Body, VEC2SIZE);
					memcpy((void*)&Velocity, Analyzer.Packets[i].Body + VEC2SIZE, VEC2SIZE);
					//critical section?
					if(!pNetworkServer->BulletsInfo.size()) AddBulletInfo.Id = 1;
					else
					{
						BulletId = min_element(pNetworkServer->BulletsInfo.begin(), pNetworkServer->BulletsInfo.end(), CompareBulletsById)->Id;
						if(BulletId > 1) AddBulletInfo.Id = BulletId - 1;
						else
						{
							BulletId = max_element(pNetworkServer->BulletsInfo.begin(), pNetworkServer->BulletsInfo.end(), CompareBulletsById)->Id;
							AddBulletInfo.Id =  BulletId + 1;
						}
					}
					pNetworkServer->BulletsInfo.push_back(AddBulletInfo);
					PhysicObject2 = new MPhysicObject2;
					PhysicObject2->SetVertex(CreateQuad(AddBulletInfo.Position.x - 0.08, AddBulletInfo.Position.y - 0.08, 0.16, 0.16));
					PhysicObject2->InitCollisionBox();
					PhysicObject2->SetVelocity(Velocity);
					pNetworkServer->Bullets.insert(pair<unsigned char, MPhysicObject2*>(AddBulletInfo.Id, PhysicObject2));
					pNetworkServer->PhysicWorld2.AddDynamicObject(pNetworkServer->Bullets[AddBulletInfo.Id]);
					break;
			}
		}
	}
	return 1;
}

DWORD WINAPI AcceptThread(void* pParam)
{
	MNetworkServer* pNetworkServer = (MNetworkServer*)pParam;
	if(!pNetworkServer)
	{
		cout<<"Can not start accept thread"<<endl;
		return 0;
	}
	cout<<"Starting Server thread"<<endl<<"> ";
    
    HANDLE thClientThread;
	DWORD dwClientThread;
    SOCKET server;
    WSADATA wsaData;
    sockaddr_in local;
    
    int wsaret=WSAStartup(0x101, &wsaData);
    if(wsaret!=0) return 0;

    local.sin_family = AF_INET; //Address family
    local.sin_addr.s_addr = INADDR_ANY; //Wild card IP address
    local.sin_port = htons((u_short)20248); //port to use

    server = socket(AF_INET, SOCK_STREAM, 0);

    if(server == INVALID_SOCKET) return 0;
    if(bind(server, (sockaddr*)&local, sizeof(local)) != 0) return 0;
    if(listen(server,10) != 0) return 0;
    
    pNetworkServer->ServerStatus = ST_SERVER_LOBBY;

    SOCKET client;
	stSocketAddress SocketAddress;
    sockaddr_in from;
    int fromlen = sizeof(sockaddr_in);
    char* cAddressFrom;
	DWORD ThreadId;
	
    while(!pNetworkServer->TerminateServer)
    {
    	memset(&client, 0, sizeof(SOCKET));
    	memset(&from, 0, sizeof(sockaddr_in));
    	memset(&client, 0, sizeof(SOCKET));
    	
        client = accept(server, (struct sockaddr*)&from, &fromlen); //wait for new connection
        if(client != INVALID_SOCKET)
		{
			cAddressFrom = inet_ntoa(from.sin_addr);
			if(pNetworkServer->Clients.size() >= CONNECTION_LIMIT)
			{
				cout<<"Connection limit. Kick "<<cAddressFrom<<endl<<"> ";
				closesocket(client);
				continue;
			}
			if(pNetworkServer->ServerStatus == ST_SERVER_PROCESS)
			{
				cout<<"Server in process. Kick "<<cAddressFrom<<endl<<"> ";
				closesocket(client);
				continue;
			}
			SocketAddress.Socket = client;
			SocketAddress.pNetworkServer = pNetworkServer;
        	CreateThread(NULL, 0, ClientsThread, (void*)&SocketAddress, 0, &ThreadId);
		}
    }
    closesocket(server);
    WSACleanup();//?

    return 1;
}

void MNetworkServer::ParseCommand(char* Command)
{
	if(!Command) return;
	if(!strlen(Command)) return;
	
	MTLVpacket TLVpacket;
	int Size;
	char temp[HEADER_SIZE + BODY_SIZE];
	DWORD dwThread;
	stQuad Quad;
	
	if(!strcmp(Command, "start"))
	{
		if(ServerStatus == ST_SERVER_PROCESS)
		{
			cout<<"Server alredy in process"<<endl;
			return;
		}
		
		int ReadyClients = 0;
		if(Clients.size() != CONNECTION_LIMIT)
		{
			cout<<"Not all connected"<<endl;
			return;
		}
		if(count_if(Clients.begin(), Clients.end(), stClientReadyCount(true)) != CONNECTION_LIMIT)
		{
			cout<<"Not all ready"<<endl;
			return;
		}
		
		if(PrepareAndSendLevelData())
		{
			cout<<"> Wait while all get start values"<<endl;
			while(count_if(Clients.begin(), Clients.end(), stClientLevelReadyCount(true)) != Clients.size())
				Sleep(1);
			memset(temp, 0, HEADER_SIZE + BODY_SIZE);
			TLVpacket.Create0x40(temp, Size, STATUS_BEGIN);
			if(SendAll(temp, Size))
			{
				thProcessThread = CreateThread(NULL, 0, ProcessThread, (void*)this, 0, &dwThread);
				ServerStatus = ST_SERVER_PROCESS;
			}
			else cout<<"> Can not start process. One of start packet not send"<<endl;
		}
		return;
	}
	cout<<"unknown command"<<endl;
}

bool MNetworkServer::PrepareAndSendLevelData()
{
	MTLVpacket TLVpacket;
	int Size, Result;
	char temp[BUFFER_SIZE];
	stLevel Level;
	unsigned char LevelNumber = 0;
	stLevelInfo AddLevelInfo;
	vector<stLevelInfo> StartLevelInfo;
	stQuad Quad;
	
	memset(temp, 0, BUFFER_SIZE);
	Level.Initialize();
	if(Level.LevelsData.size() > 1) LevelNumber = rand() % Level.LevelsData.size();
	if(Level.LevelsData[LevelNumber].SpawnPoints.size() < Clients.size()) return false;
	
	//create level
	PhysicWorld2.SetGravity(glm::vec2(0, 0));
	for(int i=0; i<Level.LevelsData[LevelNumber].Walls.size(); i++)
	{
		GroundBlock[i].SetVertex(Level.LevelsData[LevelNumber].Walls[i]);
		PhysicWorld2.AddStaticObject(&GroundBlock[i]);
	}
	
	//create players objects and send their positions
	for(int i=0; i<Clients.size(); i++)
	{
		AddLevelInfo.Id = Clients[i]->PositionInfo.Id;
		AddLevelInfo.PositionNumber = i;
		StartLevelInfo.push_back(AddLevelInfo);
		
		Clients[i]->PositionInfo.Position = Level.LevelsData[LevelNumber].SpawnPoints[i];
		Clients[i]->PositionInfo.RotationAngle = 0;
		SetQuadCH(Quad, Clients[i]->PositionInfo.Position.x, Clients[i]->PositionInfo.Position.y, 0.32f, 0.32f);
		Clients[i]->PhysicObject2.SetVertex(Quad);
		PhysicWorld2.AddDynamicObject(&Clients[i]->PhysicObject2);
	}
	
	TLVpacket.Create0x22(temp, Size, LevelNumber, &StartLevelInfo);
	return SendAll(temp, Size);
}

void MNetworkServer::MainCycle()
{	
	//let's go
	DWORD dwThread;
	TerminateServer = false;
	thAcceptThread = CreateThread(NULL, 0, AcceptThread, (void*)this, 0, &dwThread);
	
	char Cmd[12];
    while(1)
    {
    	memset(Cmd, 0, 12);
    	cout<<"> ";
		cin>>Cmd;
    	if(!strcmp(Cmd, "exit"))
		{
			TerminateServer = true;
			KickAll();
			return;
		}
		else ParseCommand(Cmd);
	}
}

void MNetworkServer::KickAll()
{
	for(it = Clients.begin(); it != Clients.end(); ++it)
	{
		TerminateThread((*it)->hClientThread, 0);//need call terminate because clients threads contains recv()
		closesocket((*it)->Socket);
	}
}
	
void MNetworkServer::Close()
{
	TerminateServer = true;
	while(WaitForSingleObject(thAcceptThread, 0) && WaitForSingleObject(thAcceptThread, 0) != WAIT_TIMEOUT)
		Sleep(1);
	if(ServerStatus == ST_SERVER_PROCESS)
	{
		while(WaitForSingleObject(thProcessThread, 0) && WaitForSingleObject(thProcessThread, 0) != WAIT_TIMEOUT)
			Sleep(1);
	}
	while(Clients.size() != 0)
		Sleep(1);
	PhysicWorld2.Close();
	for(map<unsigned char, MPhysicObject2*>::iterator it = Bullets.begin(); it != Bullets.end(); ++it)
		delete it->second;
	Bullets.clear();
	Clients.clear();
	BulletsInfo.clear();
}
