#ifndef objectH
#define objectH

class MObject
{
private:
	int BufferOffset;
	unsigned int TextureId;
protected:
	bool DirectionRight;
	stUVQuad UVQuad;
public:
	MObject();
	~MObject();
	
	//init operation
	bool SetBufferOffset(int inBufferOffset);
	bool SetTexture(unsigned int inTextureId);
	bool Set(stUVQuad UVQuad);
	
	//after this operations need update object in buffer
	bool SetVertex(stQuad Vertex);
	bool SetUV(stQuad UV);
	bool FlipTextureByX();
	bool FlipTextureByY();
	
	void SetDirectionRight(bool Right);

	stUVQuad Get();
	stQuad GetVertex();
	stQuad GetUV();
	unsigned int GetTextureId();
	int GetBufferOffset();
	
	void MoveVertex(glm::vec2 Offset);
	void SetVertex(glm::vec2 Center, glm::vec2 HalfSize);
	bool PointInObject(glm::vec2 Point);
};

#endif
