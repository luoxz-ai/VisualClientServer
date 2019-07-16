#include "Analyzer.h"

MAnalyzer::MAnalyzer()
{
	memset(Buffer, 0, BUFFER_SIZE);
	BufferSize = 0;
}

bool MAnalyzer::FillPackets(char* Data, int Size)
{
	if(!Data)
	{
		std::cout<<"Analyzer: empty data"<<std::endl;
		return false;
	}
	if(Size <= 0)
	{
		std::cout<<"Analyzer: null size"<<std::endl;
		return false;
	}
	memcpy(Buffer + BufferSize, (unsigned char*)Data, Size);
	BufferSize += Size;
	
	if(BufferSize <= HEADER_SIZE)
	{
		std::cout<<"Analyzer: small size"<<std::endl;
		return false;
	}
	
	stPacket Packet;
	int Position = 0;
	unsigned char* Pointer = NULL;
	int TotalDataSize = 0;
	int RemainDataSize;
	unsigned short int PacketSize;
	
	//clear packets each call
	Packets.clear();
	
	//search packets
	while((Pointer = (unsigned char *)memchr((void*)(Buffer + Position), SIGN, BufferSize)) != NULL && Position < BufferSize)
	{
		Position = Pointer - Buffer;
		memset((void*)&Packet, 0, sizeof(stPacket));
		memcpy((void*)&PacketSize, Buffer + Position + 2, 2);
		PacketSize += HEADER_SIZE;
		if(PacketSize <= BufferSize)
		{
			memcpy((void*)&Packet, Pointer, PacketSize);
			Packets.push_back(Packet);
			TotalDataSize += PacketSize;
			Position += PacketSize;
		}
		else break;
	}
	if(!Packets.size()) return false;
	RemainDataSize = BufferSize - TotalDataSize;
	if(RemainDataSize < 0)
	{
		std::cout<<"Analyzer: sub zero remain size"<<std::endl;
		return false;
	}
	
	//move data
	memcpy(Buffer, Buffer + TotalDataSize, RemainDataSize);
	//clear end
	memset(Buffer + RemainDataSize, 0, TotalDataSize);
	//change buffer size
	BufferSize -= TotalDataSize;
	if(BufferSize < 0)
	{
		std::cout<<"Analyzer: sub zero buffer size"<<std::endl;
		return false;
	}
	
	return true;
}
