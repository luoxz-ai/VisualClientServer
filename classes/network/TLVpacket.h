#ifndef tlvpacketH
#define tlvpacketH

#include <string.h>
#include <iostream>
#include "../../structures/Quad.h"
#include "../simpleobject/Object.h"
#include "NetworkData.h"

#define QUADSIZE 32
#define VEC2SIZE 8
#define POINTSIZE 4

#define SIGN 0x23
#define HEADER_SIZE 4
#define BODY_SIZE 1024

#define EHLO_UNKNOWN 0
#define EHLO_OK 1
#define EHLO_WRONG_VERSION 2
#define EHLO_SERVER_FULL 3

#define STATUS_UNKNOWN 0
#define STATUS_BEGIN 1
#define STATUS_PAUSE 2
#define STATUS_END 3

/*
Packet codes:
0x10 - hello from client to server +
0x11 - ehlo from server to client +
0x12 - unready/ready from client to server +
0x20 - send personal data from client to server
0x21 - send players positions from server to clients
0x22 - start level data from server
0x23 - confirmation level data
0x31 - send info about disconected client from server to client
0x40 - send begin/pause/end status from server to client
0x50 - send create bullet info
0x51 - send bullets positions from sever to clients
0x52 - send remove bullets info
*/

struct stPacket
{
	unsigned char Sign;
	unsigned char Type;
	unsigned short int Size;//unsigned short
	unsigned char Body[BODY_SIZE];
};

class MTLVpacket
{
private:
	unsigned short int TotalSize;
public:
	stPacket Packet;
	
	MTLVpacket();
	bool Recive(char* Data, unsigned char inTotalSize);
	void Null();
	void Create0x10(char* RetValue, int& RetSize, unsigned char Version);
	void Create0x11(char* RetValue, int& RetSize, unsigned char EhloResult, unsigned char Id, stServerSettings* pServerSettings);
	void Create0x12(char* RetValue, int& RetSize, bool Ready);
	void Create0x20(char* RetValue, int& RetSize, glm::vec2 Velocity, float Angle);
	void Create0x21(char* RetValue, int& RetSize, std::vector<stPositionInfo>* pPositionInfo);
	void Create0x22(char* RetValue, int& RetSize, unsigned char LevelId, std::vector<stLevelInfo>* pLevelInfo);
	void Create0x23(char* RetValue, int& RetSize);
	void Create0x31(char* RetValue, int& RetSize, unsigned char Id);
	void Create0x40(char* RetValue, int& RetSize, unsigned char Status);
	void Create0x50(char* RetValue, int& RetSize, glm::vec2 Position, glm::vec2 Velocity);
	void Create0x51(char* RetValue, int& RetSize, std::vector<stBulletInfo>* pBulletsInfo);
	void Create0x52(char* RetValue, int& RetSize, std::vector<stBulletInfo>* pBulletsInfo);
};

#endif
