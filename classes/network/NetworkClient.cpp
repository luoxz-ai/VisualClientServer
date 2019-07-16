#include "../../stdafx.h"
#include "NetworkClient.h"

MNetworkClient::MNetworkClient()
{
	Id = 0;
	Server = 0;
	thServerThread = 0;
	dwServerThread = 0;
	TerminateServer = true;
	ServerStatus = 0;
	srand(time(NULL));
}

MNetworkClient::~MNetworkClient()
{
	Id = 0;
	Server = 0;
	thServerThread = 0;
	dwServerThread = 0;
	TerminateServer = true;
	ServerStatus = 0;
}

bool MNetworkClient::Prepare()
{
	MTLVpacket TLVpacket;
	int Result;
	int Size;
	char temp[HEADER_SIZE + BODY_SIZE];
	unsigned char EhloStatus;
	
	//send helo to server
	memset(temp, 0, HEADER_SIZE + BODY_SIZE);
	TLVpacket.Create0x10(temp , Size, (unsigned char)CLIENT_VERSION);
	Result = send(Server, temp, Size, 0);
	
	//wait ehlo from server
	Result = recv(Server, temp, HEADER_SIZE + BODY_SIZE, 0);
	if(Result <= 0)
	{
		cout<<"Prepare: recv failed"<<endl;
		return false;
	}
	if(TLVpacket.Recive(temp, Result))
	{
		if(TLVpacket.Packet.Type == 0x11) EhloStatus = TLVpacket.Packet.Body[0];
		if(EhloStatus != EHLO_OK)
		{
			cout<<"Bad ehlo: "<<(unsigned int)TLVpacket.Packet.Body[0]<<endl;
			return false;
		}
		else
		{
			Id = TLVpacket.Packet.Body[1];
			//settings
		}
	}
	
	//send ready
	memset(temp, 0, HEADER_SIZE + BODY_SIZE);
	TLVpacket.Create0x12(temp , Size, true);
	Result = send(Server, temp, Size, 0);
	if(Result != Size)
	{
		cout<<"Prepare: send failed"<<endl;
		return false;
	}
	
	return true;
}

bool MNetworkClient::AnalyzePacket(stPacket* inpPacket)
{
	if(!inpPacket) return false;
	
	int Result;
	int Size;
	char temp[HEADER_SIZE + BODY_SIZE];
	stPositionInfo PositionInfo;
	MTLVpacket TLVpacket;
	stQuad Quad, Vertex;
	MObject Object;
	glm::vec2 Point;
	unsigned short int LevelId;
	unsigned short int PlayersCount;
	unsigned short int PlayerId;
	unsigned short int SpawnPosId;
	unsigned short int BulletId;
	bool Status;
	
	switch(inpPacket->Type)
	{
		case 0x22://start level data
			//inti level
			cout<<"0x22: "<<(unsigned short int)inpPacket->Body[0]<<endl;
			LevelId = (unsigned short int)inpPacket->Body[0];
			PlayersCount = (unsigned short int)inpPacket->Body[1];
			if(LevelId > Level.LevelsData.size()) 
			{
				cout<<"Wrong level id"<<endl;
				return false;
			}
			//init players
			for(int i=0; i<PlayersCount; i++)
			{
				PlayerId = (unsigned short int)inpPacket->Body[2 + i * 2];
				SpawnPosId = (unsigned short int)inpPacket->Body[3 + i * 2];
				Point = Level.LevelsData[LevelId].SpawnPoints[SpawnPosId];
				SetQuadCH(Vertex, Point.x, Point.y, 0.32, 0.32);
				PlayersObjects[i]->SetVertex(Vertex);
			}
			LevelNumber = LevelId;
			cout<<"Set level: "<<LevelNumber<<endl;
			//confirm level data recive
			TLVpacket.Create0x23(temp, Size);
			Result = send(Server, temp, Size, 0);
			if(Result != Size) cout<<"Send level confirm failed"<<endl;
			//send recive confirmation with own data
			TLVpacket.Create0x20(temp, Size, MainVelocity, MainRotation);
			Result = send(Server, temp, Size, 0);
			if(Result != Size) cout<<"Send velocity confirm failed"<<endl;
			break;
			
		case 0x21://total data
			if(!PlayersObjects.size()) break;
			//return data to visual part of app
			for(unsigned short int i=0; i<inpPacket->Body[0]; i++)
			{
				if(!PlayersObjects[i]) break;
				PositionInfo.Id = inpPacket->Body[1 + i * (VEC2SIZE + POINTSIZE + 1)];
				memcpy((void*)&PositionInfo.Position, inpPacket->Body + 2 + i * (VEC2SIZE + POINTSIZE + 1), VEC2SIZE);
				memcpy((void*)&PositionInfo.RotationAngle, inpPacket->Body + 2 + i * (VEC2SIZE + POINTSIZE + 1) + VEC2SIZE, POINTSIZE);
				SetQuadCH(Quad, PositionInfo.Position.x, PositionInfo.Position.y, 0.32, 0.32);
				PlayersObjects[i]->SetVertex(QuadRotate(Quad, PositionInfo.RotationAngle));
				if(PositionInfo.Id == Id) SetMainCenter(PositionInfo.Position);
			}
			//send recive confirmation with own data
			TLVpacket.Create0x20(temp, Size, MainVelocity, MainRotation);
			Result = send(Server, temp, Size, 0);
			if(Result != Size) cout<<"Send velocity confirm failed"<<endl;
			//nulling velocity
			MainVelocity.x = MainVelocity.y = 0;
			break;
			
		case 0x31://disconnected client
			cout<<"Disonect client "<<(unsigned int)inpPacket->Body[0]<<endl;
			PlayersBuffer.RemoveObject(PlayersObjects[inpPacket->Body[0]]);
			//PlayersObjects.erase(inpPacket->Body[0]);//suspisious
			break;
			
		case 0x40://status
			ServerStatus = (unsigned int)inpPacket->Body[0];
			if(ServerStatus > STATUS_END) cout<<"0x40: Unknow status"<<endl;
			else cout<<"0x40: Process status: "<<ServerStatus<<endl;
			break;
			
		case 0x51://bullets total info
			Status = false;
			for(unsigned short int i=0; i<inpPacket->Body[0]; i++)
			{
				BulletId = *(inpPacket->Body + 1 + i * (VEC2SIZE + 1));
				memcpy((void*)&Point, inpPacket->Body + 2 + i * (VEC2SIZE + 1), VEC2SIZE);
				//create or update bullets
				if(BulletsObjects.find(BulletId) == BulletsObjects.end())
				{
					BulletsObjects.insert(pair<unsigned char, MObject*>(BulletId, new MObject));
					BulletsObjects[BulletId]->SetVertex(CreateQuad(Point.x - 0.08, Point.y - 0.08, 0.16, 0.16));
					BulletsObjects[BulletId]->SetUV(CreateQuad(0, 0, 1, 1));
					BulletsObjects[BulletId]->SetTexture(txBullet->Id);
					BulletsToAdd.push_back(BulletId);
					cout<<"Bullet pos: "<<Point.x<<" "<<Point.y<<endl;
				}
				//else 
				//{
				//	BulletsObjects[BulletId]->SetVertex(CreateQuad(Point.x - 0.08, Point.y - 0.08, 0.16, 0.16));
				//	cout<<"0x51: update"<<endl;	
				//}
			}
			NeedUpdateBulletsBuffer = Status;
			break;
			
		case 0x52://bullets remove info
			for(unsigned short int i=0; i<inpPacket->Body[0]; i++)
			{
				BulletId = inpPacket->Body[1 + i];
				if(BulletsObjects.find(BulletId) != BulletsObjects.end()) BulletsBuffer.RemoveObject(BulletsObjects[BulletId]);
			}
			NeedUpdateBulletsBuffer = true;
			break;
			
		default:
			cout<<"Unknown packet: "<<(int)inpPacket->Sign<<" "<<(int)inpPacket->Type<<" "<<inpPacket->Size<<": "<<endl;
			break;
	}
	
	return true;
}

DWORD WINAPI ServerThread(void* pParam)
{
	MNetworkClient* pNetworkClient = (MNetworkClient*)pParam;
	if(!pNetworkClient)
	{
		cout<<"Can not start server thread"<<endl;
		return 0;
	}
	cout<<"Start server thread"<<endl;

	int Result;
	int Size;
	char temp[HEADER_SIZE + BODY_SIZE];
	MAnalyzer Analyzer;
	while(!pNetworkClient->TerminateServer)
	{
		memset(temp, 0, HEADER_SIZE + BODY_SIZE);
		Result = recv(pNetworkClient->Server, temp, HEADER_SIZE + BODY_SIZE, 0);
		if(Result <= 0)
		{
			pNetworkClient->TerminateServer = true;
			Analyzer.Packets.clear();
			break;
		}
		Analyzer.FillPackets(temp, Result);
		for(int i=0; i<Analyzer.Packets.size(); i++)
		{
			pNetworkClient->AnalyzePacket(&Analyzer.Packets[i]);
		}
		Analyzer.Packets.clear();
		Sleep(1);
	}
	cout<<"Server thread ended"<<endl;
	closesocket(pNetworkClient->Server);
	pNetworkClient->NeedCloseWindow = true;
	return 0;
}

bool MNetworkClient::ConnectToServer(const char* incServerName, const char* incServerPort)
{
	WSADATA wsaData;
	int wsaret = WSAStartup(0x101, &wsaData);
    if(wsaret != 0)
	{
		cout<<"Can not initialize sockets"<<endl;
		return false;
	}
	
	if(!incServerName || !incServerPort)
	{
		cout<<"Empty ip or port"<<endl;
		return false;
	}
    
    int Result;
    addrinfo* result = NULL;
    addrinfo hints;
    
    memset(&hints, 0, sizeof(hints));
    Result = getaddrinfo(incServerName, incServerPort, &hints, &result);
    if(Result != 0)
    {
		cout<<"Error: Can no get address info: "<<Result<<endl;
		return false;
	}
	Server = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if(Server == INVALID_SOCKET)
    {
    	cout<<"Socket error: "<<WSAGetLastError()<<endl;
    	return false;
    }
    Result = connect(Server, result->ai_addr, (int)result->ai_addrlen);
	if(Result == SOCKET_ERROR)
	{
		cout<<"Connect error: "<<WSAGetLastError()<<endl;
		return false;
	}
	freeaddrinfo(result);

	if(!Prepare()) return false;
	if(!Initialize()) return false;//graphic part
	
	Sleep(100);
	TerminateServer = false;
	thServerThread = CreateThread(NULL, 0, ServerThread, (void*)this, 0, &dwServerThread);
	
	return true;
}

bool MNetworkClient::GetTerminateServer()
{
	return TerminateServer;
}

int MNetworkClient::GetServerStatus()
{
	return ServerStatus;
}

void MNetworkClient::Close()
{
	if(!TerminateServer)
	{
		TerminateServer = true;
		while(WaitForSingleObject(thServerThread, 0) != WAIT_TIMEOUT)
			Sleep(1);
	}
	WSACleanup();
	MGraphicClient::Close();
}

void MNetworkClient::OnMouseClick(glm::vec2 Position)
{
	if(ServerStatus != STATUS_BEGIN) return;
	MTLVpacket TLVpacket;
	int Size;
	int Result;
	char temp[HEADER_SIZE + BODY_SIZE];
	TLVpacket.Create0x50(temp, Size, Position, glm::vec2(0,0));
	Result = send(Server, temp, Size, 0);
	if(Result != Size) cout<<"Send new bullet failed"<<endl;
	cout<<"0x50: out: "<<Position.x<<" "<<Position.y<<endl;
}
