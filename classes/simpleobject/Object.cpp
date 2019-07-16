#include "../../stdafx.h"
#include "Object.h"

MObject::MObject()
{
	BufferOffset = -1;
	NullUVQuad(UVQuad);
	TextureId = 0;
	DirectionRight = true;
}

MObject::~MObject()
{
	BufferOffset = -1;
	NullUVQuad(UVQuad);
	TextureId = 0;
	DirectionRight = true;
}

bool MObject::SetBufferOffset(int inBufferOffset)
{
	BufferOffset = inBufferOffset;
	return true;
}

bool MObject::SetTexture(unsigned int inTextureId)
{
	if(!inTextureId)
	{
		LogFile<<"Object: Wrong texture id"<<endl;
		return false;
	}
	TextureId = inTextureId;
	return true;
}

bool MObject::SetVertex(stQuad Vertex)
{
	SetUVQuad(UVQuad, Vertex, GetUVQuad(UVQuad));
	return true;
}

bool MObject::SetUV(stQuad UV)
{
	SetUVQuad(UVQuad, GetVertexQuad(UVQuad), UV);
	return true;
}

bool MObject::Set(stUVQuad inUVQuad)
{
	//if(BufferOffset < 0) return false;
	UVQuad = inUVQuad;
	return true;
}
	
bool MObject::FlipTextureByX()
{
	if(BufferOffset < 0) return false;
	swap(UVQuad.p[0][1], UVQuad.p[3][1]);
	swap(UVQuad.p[1][1], UVQuad.p[2][1]);
	return true;
}

bool MObject::FlipTextureByY()
{
	if(BufferOffset < 0) return false;
	swap(UVQuad.p[0][1], UVQuad.p[1][1]);
	swap(UVQuad.p[2][1], UVQuad.p[3][1]);
	return true;
}

void MObject::SetDirectionRight(bool Right)
{
	DirectionRight = Right;
}

stQuad MObject::GetVertex()
{
	return GetVertexQuad(UVQuad);
}

stQuad MObject::GetUV()
{
	return GetUVQuad(UVQuad);
}

unsigned int MObject::GetTextureId()
{
	return TextureId;
}

int MObject::GetBufferOffset()
{
	return BufferOffset;
}

stUVQuad MObject::Get()
{
	return UVQuad;
}

void MObject::MoveVertex(glm::vec2 Offset)
{
	MoveUVQuad(UVQuad, Offset);
}

void MObject::SetVertex(glm::vec2 Center, glm::vec2 HalfSize)
{
	SetUVQuadVertexCH(UVQuad, Center.x, Center.y, HalfSize.x, HalfSize.y);
}

bool MObject::PointInObject(glm::vec2 Point)
{
	return (Point.x > UVQuad.p[0][0].x && Point.x < UVQuad.p[2][0].x && Point.y > UVQuad.p[0][0].y && Point.y < UVQuad.p[2][0].y);
}
