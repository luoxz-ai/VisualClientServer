#ifndef analyzerH
#define analyzerH

#include <iostream>
#include "TLVpacket.h"

#define BUFFER_SIZE 1024

class MAnalyzer
{
private:
	unsigned char Buffer[BUFFER_SIZE];
public:
	int BufferSize;
	std::vector<stPacket> Packets;
	
	MAnalyzer();
	bool FillPackets(char* Data, int Size);
};

#endif
