#ifndef graphicclientH
#define graphicclientH

#include "../image/TextureLoader.h"
#include "../system/Shader.h"
#include "../system/Scene.h"
#include "../buffer/ObjectBuffer.h"
#include "../buffer/StaticBuffer.h"
#include "../../structures/FPSController.h"
#include "../network/NetworkData.h"

#define ONE 1

class MGraphicClient
{
protected:
	int LevelNumber;
	stLevel Level;
	map<unsigned char, MStaticBuffer*> LevelsBuffers;
	map<unsigned char, MObject*> PlayersObjects;
	map<unsigned char, MObject*> BulletsObjects;
	vector<unsigned char> BulletsToAdd;
	MObjectBuffer PlayersBuffer;
	MObjectBuffer BulletsBuffer;
	
	stTexture* txWall;
	stTexture* txPlayer;
	stTexture* txPlayerSpawn;
	stTexture* txAmmoSpawn;
	stTexture* txBullet;
	
	bool NeedCloseWindow;
	glm::vec2 MainVelocity;
	float MainRotation;
	glm::vec2 MainCenter;
	bool NeedUpdateBulletsBuffer;
	
	void SetMainCenter(glm::vec2 Center);
	virtual void OnMouseClick(glm::vec2 Position);
	
private:
	GLFWwindow* window;
	HANDLE thRenderThread;
	int WindowWidth;
	int WindowHeight;
	bool keys[1024];
	
	MShader Shader;
	MScene Scene;
	MTextureLoader TextureLoader;
	stFPSController FPSController;
	
	glm::vec4 whiteColor;
	double mx;
	double my;
	glm::vec2 MouseSceneCoord;
	glm::vec2 MainDirection;
	
	static void WindowErrorCallback(int error, const char* description);
	static void WindowKeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void WindowMouseCallback(GLFWwindow* window, double x, double y);
	static void WindowMouseClickCallback(GLFWwindow* window, int button, int action, int mods);
	bool IsOpenglSupported();
	void KeyboardActions();
	void RenderStep();
public:
	MGraphicClient();
	~MGraphicClient();
	bool Initialize();
	void Render();
	void Close();
};

#endif
