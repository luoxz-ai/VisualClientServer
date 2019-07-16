#include "TLVpacket.h"

MTLVpacket::MTLVpacket()
{
	Packet.Sign = 0;
	Packet.Type = 0;
	Packet.Size = 0;
	memset(Packet.Body, 0, BODY_SIZE);
}

bool MTLVpacket::Recive(char* Data, unsigned char inTotalSize)
{
	if(!Data)
	{
		std::cout<<"Empty data"<<std::endl;
		return false;
	}
	if(inTotalSize < HEADER_SIZE)
	{
		std::cout<<"Too small size"<<std::endl;
		return false;
	}
	if(inTotalSize >= HEADER_SIZE + BODY_SIZE)
	{
		std::cout<<"Too big size"<<std::endl;
		return false;
	}
	if(Data[0] != SIGN)
	{
		std::cout<<"Wrong sign"<<std::endl;
		return false;
	}
	
	unsigned short int PacketSize;
	memcpy((void*)&PacketSize, Data + 2, 2);
	if(PacketSize + HEADER_SIZE != inTotalSize)
	{
		std::cout<<"Wrong size: "<<PacketSize<<" "<<inTotalSize<<std::endl;
		return false;
	}
	
	TotalSize = inTotalSize;
	
	Packet.Sign = Data[0];
	Packet.Type = Data[1];
	Packet.Size = PacketSize;
	memcpy(Packet.Body, Data + HEADER_SIZE, TotalSize - HEADER_SIZE);
}

void MTLVpacket::Null()
{
	Packet.Sign = SIGN;
	Packet.Type = 0;
	Packet.Size = 0;
	memset(Packet.Body, 0, BODY_SIZE);
	TotalSize = 0;
}

void MTLVpacket::Create0x10(char* RetValue, int& RetSize, unsigned char Version)
{
	Null();
	Packet.Type = 0x10;
	Packet.Size = 1;
	Packet.Body[0] = Version;
	TotalSize = HEADER_SIZE + Packet.Size;
	
	memcpy(RetValue, (char*)&Packet, TotalSize);
	RetSize = TotalSize;
}

void MTLVpacket::Create0x11(char* RetValue, int& RetSize, unsigned char EhloResult, unsigned char Id, stServerSettings* pServerSettings)
{
	Null();
	Packet.Type = 0x11;
	Packet.Body[0] = (unsigned char)EhloResult;
	if(EhloResult != EHLO_OK) Packet.Size = 1;
	else
	{
		Packet.Size = 4 + sizeof(stPlayerSettings) + sizeof(stBulletSettings);
		Packet.Body[1] = Id;
		Packet.Body[2] = pServerSettings->Version;
		Packet.Body[3] = pServerSettings->ClientsLimit;
		memcpy(Packet.Body + 4, (void*)&pServerSettings->PlayerSettings, sizeof(stPlayerSettings));
		memcpy(Packet.Body + 4 + sizeof(stPlayerSettings), (void*)&pServerSettings->BulletSettings, sizeof(stBulletSettings));
	}
	
	TotalSize = HEADER_SIZE + Packet.Size;
	
	memcpy(RetValue, (char*)&Packet, TotalSize);
	RetSize = TotalSize;
}

void MTLVpacket::Create0x12(char* RetValue, int& RetSize, bool Ready)
{
	Null();
	Packet.Type = 0x12;
	Packet.Size = 1;
	Packet.Body[0] = (unsigned char)Ready;
	TotalSize = HEADER_SIZE + Packet.Size;
	
	memcpy(RetValue, (char*)&Packet, TotalSize);
	RetSize = TotalSize;
}

void MTLVpacket::Create0x20(char* RetValue, int& RetSize, glm::vec2 Velocity, float Angle)
{
	Null();
	Packet.Type = 0x20;
	Packet.Size = VEC2SIZE + POINTSIZE;
	memcpy(Packet.Body, (void*)&Velocity, VEC2SIZE);
	memcpy(Packet.Body + VEC2SIZE, (void*)&Angle, POINTSIZE);
	TotalSize = HEADER_SIZE + Packet.Size;
	
	memcpy(RetValue, (char*)&Packet, TotalSize);
	RetSize = TotalSize;
}

void MTLVpacket::Create0x21(char* RetValue, int& RetSize, std::vector<stPositionInfo>* pPositionInfo)
{
	unsigned int Count = pPositionInfo->size();
	unsigned int PartSize = 1 + VEC2SIZE + POINTSIZE;
	Null();
	Packet.Type = 0x21;
	Packet.Size = 1 + Count * PartSize;
	Packet.Body[0] = Count;
	for(int i=0; i<Count; i++)
	{
		*(Packet.Body + 1 + i * PartSize) = pPositionInfo->at(i).Id;
		memcpy(Packet.Body + 2 + i * PartSize, (void*)&pPositionInfo->at(i).Position, VEC2SIZE);
		memcpy(Packet.Body + 2 + i * PartSize + VEC2SIZE, (void*)&pPositionInfo->at(i).RotationAngle, POINTSIZE);
	}
	TotalSize = HEADER_SIZE + Packet.Size;
	
	memcpy(RetValue, (char*)&Packet, TotalSize);
	RetSize = TotalSize;
}

void MTLVpacket::Create0x22(char* RetValue, int& RetSize, unsigned char LevelId, std::vector<stLevelInfo>* pLevelInfo)
{
	Null();
	if(!pLevelInfo) return;
	unsigned char Count = pLevelInfo->size();
	
	Packet.Type = 0x22;
	Packet.Size = 2 + Count * 2;
	Packet.Body[0] = LevelId;
	Packet.Body[1] = Count;
	for(int i=0; i<Count; i++)
	{
		Packet.Body[2 + i * 2] = pLevelInfo->at(i).Id;
		Packet.Body[2 + i * 2 + 1] = pLevelInfo->at(i).PositionNumber;
	}
	TotalSize = HEADER_SIZE + Packet.Size;
	
	memcpy(RetValue, (char*)&Packet, TotalSize);
	RetSize = TotalSize;
}

void MTLVpacket::Create0x23(char* RetValue, int& RetSize)
{
	Null();
	Packet.Type = 0x23;
	Packet.Size = 1;
	Packet.Body[0] = 1;
	TotalSize = HEADER_SIZE + Packet.Size;
	
	memcpy(RetValue, (char*)&Packet, TotalSize);
	RetSize = TotalSize;
}

void MTLVpacket::Create0x31(char* RetValue, int& RetSize, unsigned char Id)
{
	Null();
	Packet.Type = 0x31;
	Packet.Size = 1;
	Packet.Body[0] = Id;
	TotalSize = HEADER_SIZE + Packet.Size;
	
	memcpy(RetValue, (char*)&Packet, TotalSize);
	RetSize = TotalSize;
}

void MTLVpacket::Create0x40(char* RetValue, int& RetSize, unsigned char Status)
{
	Null();
	Packet.Type = 0x40;
	Packet.Size = 1;
	Packet.Body[0] = Status;
	TotalSize = HEADER_SIZE + Packet.Size;
	
	memcpy(RetValue, (char*)&Packet, TotalSize);
	RetSize = TotalSize;
}

void MTLVpacket::Create0x50(char* RetValue, int& RetSize, glm::vec2 Position, glm::vec2 Velocity)
{
	Null();
	Packet.Type = 0x50;
	Packet.Size = VEC2SIZE * 2;
	memcpy(Packet.Body, (void*)&Position, VEC2SIZE);
	memcpy(Packet.Body + VEC2SIZE, (void*)&Velocity, VEC2SIZE);
	TotalSize = HEADER_SIZE + Packet.Size;
	
	memcpy(RetValue, (char*)&Packet, TotalSize);
	RetSize = TotalSize;
}

void MTLVpacket::Create0x51(char* RetValue, int& RetSize, std::vector<stBulletInfo>* pBulletsInfo)
{
	unsigned int Count = pBulletsInfo->size();
	Null();
	Packet.Type = 0x51;
	Packet.Size = 1 + (VEC2SIZE + 1) * Count;
	Packet.Body[0] = Count;
	for(int i=0; i<Count; i++)
	{
		*(Packet.Body + 1 + i * (VEC2SIZE + 1)) = pBulletsInfo->at(i).Id;
		memcpy(Packet.Body + 2 + i * (VEC2SIZE + 1), (void*)&pBulletsInfo->at(i).Position, VEC2SIZE);
	}
	TotalSize = HEADER_SIZE + Packet.Size;
	
	memcpy(RetValue, (char*)&Packet, TotalSize);
	RetSize = TotalSize;
}

void MTLVpacket::Create0x52(char* RetValue, int& RetSize, std::vector<stBulletInfo>* pBulletsInfo)
{
	unsigned int Count = pBulletsInfo->size();
	Null();
	Packet.Type = 0x52;
	Packet.Size = Count;
	Packet.Body[0] = Count;
	for(int i=0; i<Count; i++)
	{
		Packet.Body[i + 1] = pBulletsInfo->at(i).Id;
	}
	TotalSize = HEADER_SIZE + Packet.Size;
	
	memcpy(RetValue, (char*)&Packet, TotalSize);
	RetSize = TotalSize;
}
