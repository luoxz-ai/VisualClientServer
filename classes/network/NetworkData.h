#ifndef networkdataH
#define networkdataH

#include <vector>

struct stPlayerSettings
{
	unsigned char Health;
	glm::vec2 Velocity;
	glm::vec2 HalfSize;
};

struct stBulletSettings
{
	unsigned char Damage;
	glm::vec2 Velocity;
	glm::vec2 HalfSize;
};

struct stServerSettings
{
	unsigned char Version;
	unsigned char ClientsLimit;
	stPlayerSettings PlayerSettings;
	stBulletSettings BulletSettings;
};

struct stPositionInfo
{
	unsigned char Id;
	glm::vec2 Position;
	float RotationAngle;
	stServerValue()
	{
		Id = 0;
		Position = glm::vec2(0,0);
	}
	stServerValue(unsigned char inId, glm::vec2 inPosition, float inRotationAngle)
	{
		inPosition = inPosition;
		RotationAngle = inRotationAngle;
	}
};

struct stPositionInfoById
{
	stPositionInfo PositionInfo;
	stPositionInfoById(unsigned char inId)
	{
		PositionInfo.Id = inId;
	}
	bool operator () (stPositionInfo inPositionInfo)
	{
		return PositionInfo.Id == inPositionInfo.Id;
	}
};

struct stBulletInfo
{
	unsigned char Id;
	glm::vec2 Position;
};

struct stBulletById
{
	stBulletInfo BulletInfo;
	stBulletById(unsigned char inId)
	{
		BulletInfo.Id = inId;
	}
	bool operator () (stBulletInfo inBulletInfo)
	{
		return BulletInfo.Id == inBulletInfo.Id;
	}
};

bool CompareBulletsById(stBulletInfo A, stBulletInfo B);

struct stLevelInfo
{
	unsigned char Id;
	unsigned char PositionNumber;
};

struct stLevelData
{
	char Name[16];
	std::vector<stQuad> Walls;
	std::vector<glm::vec2> SpawnPoints;
	std::vector<glm::vec2> AmmoPoints;
	
	stLevelData()
	{
		memset(Name, 0, 16);
	}
	
	void Clear()
	{
		memset(Name, 0, 16);
		Walls.clear();
		SpawnPoints.clear();
		AmmoPoints.clear();
	}
};

struct stLevel
{
	bool Initialized;
	std::vector<stLevelData> LevelsData;
	stLevel()
	{
		Initialized = false;
	}
	void Initialize()
	{
		stLevelData AddLevelData;
		//level 0
		//set name
		strcat(AddLevelData.Name, "Demo000");
		//set walls
		AddLevelData.Walls.push_back(CreateQuad(0, 0, 8, 0.32));
		AddLevelData.Walls.push_back(CreateQuad(0, 5.68, 8, 0.32));
		AddLevelData.Walls.push_back(CreateQuad(0, 0.32, 0.32, 5.36));
		AddLevelData.Walls.push_back(CreateQuad(7.68, 0.32, 0.32, 5.36));
		//set spawn points
		AddLevelData.SpawnPoints.push_back(glm::vec2(0.96, 0.96));//0.64, 0.64
		AddLevelData.SpawnPoints.push_back(glm::vec2(7.04, 5.04));
		//set ammo points
		AddLevelData.AmmoPoints.push_back(glm::vec2(7.36, 0.64));
		AddLevelData.AmmoPoints.push_back(glm::vec2(0.64, 5.36));
		//add to level
		LevelsData.push_back(AddLevelData);
		//clear
		AddLevelData.Clear();
	}
};

#endif
