#include "../../stdafx.h"
#include "StaticBuffer.h"

MStaticBuffer::MStaticBuffer()
{
	BufferId = -1;
	BindNumber = 0;
	pTexture = NULL;
	BufferSize = 0;
}

MStaticBuffer::~MStaticBuffer()
{
	BufferId = -1;
	BindNumber = 0;
	pTexture = NULL;
	BufferSize = 0;
}

bool MStaticBuffer::Initialize(stTexture* inpTexture)
{
	if(!inpTexture)
	{
		LogFile<<"BoxBuffer2: Empty texture given"<<endl;
		return false;
	}
	pTexture = inpTexture;
	
	glGenBuffers(1, &BufferId);
	GLenum error = glGetError();
	if(error != GL_NO_ERROR)
	{
		LogFile<<"StaticBuffer: "<<(char*)gluErrorString(error)<<" "<<error<<endl;
		return false;
	}
	
	return true;
}

void MStaticBuffer::SetBindNumber(unsigned char Number)
{
	BindNumber = Number;
}

bool MStaticBuffer::AddUVQuad(float vx, float vy, float vw, float vh, float tx, float ty, float tw, float th)
{
	stUVQuad UVQuad;
	SetUVQuad(UVQuad, vx,vy,vw,vh, tx,ty,tw,th);
	UVQuads.push_back(UVQuad);
	return true;
}

bool MStaticBuffer::AddUVQuad(stQuad Vertex, stQuad UV)
{
	stUVQuad UVQuad;
	SetUVQuad(UVQuad, Vertex, UV);
	UVQuads.push_back(UVQuad);
	return true;
}

bool MStaticBuffer::AddUVQuad(stUVQuad* UVQuad)
{
	if(!UVQuad) return false;
	UVQuads.push_back(*UVQuad);
	return true;
}

bool MStaticBuffer::Dispose()
{
	if(BufferId <= 0)
	{
		LogFile<<"StaticBuffer: Not initialized"<<endl;
		return false;
	}
	BufferSize = UVQuads.size();
	glBindBuffer(GL_ARRAY_BUFFER, BufferId);
	glBufferData(GL_ARRAY_BUFFER, BufferSize * sizeof(stUVQuad), &UVQuads[0], GL_STATIC_DRAW);
	UVQuads.clear();
	
	return true;
}

void MStaticBuffer::Draw()
{
	glActiveTexture(GL_TEXTURE0 + BindNumber);
	glBindTexture(GL_TEXTURE_2D, pTexture->Id);
	glBindBuffer(GL_ARRAY_BUFFER, BufferId);
	glVertexAttribPointer(SHR_LAYOUT_VERTEX, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, BufferId);
	glVertexAttribPointer(SHR_LAYOUT_UV, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)8);
	glDrawArrays(GL_QUADS, 0, 4 * BufferSize);
}
 
void MStaticBuffer::Begin()
{
	glEnableVertexAttribArray(SHR_LAYOUT_VERTEX);
	glEnableVertexAttribArray(SHR_LAYOUT_UV);
}

void MStaticBuffer::End()
{
	glDisableVertexAttribArray(SHR_LAYOUT_VERTEX);
	glDisableVertexAttribArray(SHR_LAYOUT_UV);
}

void MStaticBuffer::Clear()
{
	glBindBuffer(GL_ARRAY_BUFFER, BufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(stUVQuad), NULL, GL_STATIC_DRAW);
	UVQuads.clear();
	BufferSize = 0;
}

void MStaticBuffer::Close()
{
	Clear();
	if(BufferId > 0) glDeleteBuffers(1, &BufferId);
}
