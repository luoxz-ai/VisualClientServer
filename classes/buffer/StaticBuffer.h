#ifndef staticbufferH
#define staticbufferH

/*
static buffer
local data copy to buffer and then clearing
data in buffer can not be changed
*/

class MStaticBuffer
{
protected:
	GLuint BufferId;
	int BufferSize;
	vector<stUVQuad> UVQuads;
	unsigned int BindNumber;
	stTexture* pTexture;
public:
	MStaticBuffer();
	~MStaticBuffer();
	bool Initialize(stTexture* inpTexture);
	void SetBindNumber(unsigned char Number);
	bool AddUVQuad(float vx, float vy, float vw, float vh, float tx, float ty, float tw, float th);
	bool AddUVQuad(stQuad Vertex, stQuad UV);
	bool AddUVQuad(stUVQuad* UVQuad);
	bool Dispose();
	void Draw();
	void Begin();
	void End();
	void Clear();
	void Close();
};

#endif
