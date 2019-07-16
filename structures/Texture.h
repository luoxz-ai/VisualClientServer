#ifndef textureH
#define textureH

struct stTexture
{
    unsigned int Id;
    glm::vec2 uvTilePosOffset;
    glm::vec2 uvTileSizeOffset;
    stTexture()
    {
    	Id = 0;
    	uvTilePosOffset = glm::vec2(0.0f, 0.0f);
    	uvTileSizeOffset = glm::vec2(0.0f, 0.0f);
	}
	void SetTiles(int ImageWidth, int ImageHeight)
	{
		uvTilePosOffset = glm::vec2((float)(0.5 / ImageWidth), (float)(0.5 / ImageHeight));
    	uvTileSizeOffset = glm::vec2(uvTilePosOffset.x * 2, uvTilePosOffset.y * 2);
	}
};

#endif
