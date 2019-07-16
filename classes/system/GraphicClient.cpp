#include "../../stdafx.h"
#include "GraphicClient.h"

void MGraphicClient::WindowErrorCallback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

void MGraphicClient::WindowKeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	MGraphicClient* pGraphicClient = static_cast<MGraphicClient*>(glfwGetWindowUserPointer(window));
	if(!pGraphicClient)
	{
		LogFile<<"Can not start keyboard callback"<<endl;
		return;
	}
	
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		//glfwSetWindowShouldClose(window, GLFW_TRUE);
		pGraphicClient->NeedCloseWindow = true;
		return;
	}
    
    if(action == GLFW_PRESS)
    	pGraphicClient->keys[key] = true;
    else if (action == GLFW_RELEASE)
    	pGraphicClient->keys[key] = false;
}

void MGraphicClient::WindowMouseCallback(GLFWwindow* window, double x, double y)
{
	MGraphicClient* pGraphicClient = static_cast<MGraphicClient*>(glfwGetWindowUserPointer(window));
	if(!pGraphicClient)
	{
		LogFile<<"Can not start keyboard callback"<<endl;
		return;
	}
	
	pGraphicClient->MouseSceneCoord = pGraphicClient->Scene.WindowPosToWorldPos(x, y);
}

void MGraphicClient::WindowMouseClickCallback(GLFWwindow* window, int button, int action, int mods)
{
	MGraphicClient* pGraphicClient = static_cast<MGraphicClient*>(glfwGetWindowUserPointer(window));
	if(!pGraphicClient)
	{
		LogFile<<"Can not start keyboard callback"<<endl;
		return;
	}
	
	if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) pGraphicClient->OnMouseClick(pGraphicClient->MouseSceneCoord);
}

void MGraphicClient::Render()
{
	//while(!glfwWindowShouldClose(window))
	while(!NeedCloseWindow)
	{
		FPSController.FrameStep(glfwGetTime());
	  	FPSController.FrameCheck();
	   	KeyboardActions();
	   	RenderStep();
		glfwSwapBuffers(window);
	    glfwPollEvents();
	}
}

bool MGraphicClient::IsOpenglSupported()
{
	//get opengl data (here was fall on suspisious notebook)
	string OpenGLVersion = (char*)glGetString(GL_VERSION);
	string OpenGLVendor = (char*)glGetString(GL_VENDOR);
	string OpenGLRenderer = (char*)glGetString(GL_RENDERER);
	string ShadersVersion = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
	LogFile<<"Window: OpenGL version: "<<OpenGLVersion<<endl;
	LogFile<<"Window: OpenGL vendor: "<<OpenGLVendor<<endl;
	LogFile<<"Window: OpenGL renderer: "<<OpenGLRenderer<<endl;
	LogFile<<"Window: Shaders version: "<<ShadersVersion<<endl;
	
	float VersionOGL, VersionSHD;
	sscanf(OpenGLVersion.c_str(), "%f", &VersionOGL);
	if(VersionOGL < 3.0f)
	{
		LogFile<<"Window: Old version of OpenGL. Sorry"<<endl;
		return false;
	}
	sscanf(ShadersVersion.c_str(), "%f", &VersionSHD);
	if(VersionSHD < 3.3f)
	{
		LogFile<<"Window: Old version of shaders. Sorry"<<endl;
		return false;
	}
}

void MGraphicClient::KeyboardActions()
{
	MainVelocity = glm::vec2(0, 0);
	if(keys['A']) MainVelocity.x = -0.02;
	if(keys['D']) MainVelocity.x = 0.02;
	if(keys['W']) MainVelocity.y = 0.02;
	if(keys['S']) MainVelocity.y = -0.02;
}

void MGraphicClient::SetMainCenter(glm::vec2 Center)
{
	MainCenter = Center;
	MainDirection = glm::normalize(MouseSceneCoord - MainCenter);
	MainRotation = atan2(MainDirection.y, MainDirection.x);
}

void MGraphicClient::OnMouseClick(glm::vec2 Position) {}

void MGraphicClient::RenderStep()
{
	glClear(GL_COLOR_BUFFER_BIT);
	if(LevelNumber < 0) return;
	if(BulletsToAdd.size())
	{
		for(int i=0; i<BulletsToAdd.size(); i++)
			if(!BulletsBuffer.AddObject(BulletsObjects[BulletsToAdd[i]])) cout<<"Bullet add failed"<<endl; else cout<<"Bullet add to buffer"<<endl;
		BulletsToAdd.clear();
		BulletsBuffer.DisposeAll();
	}
	
	PlayersBuffer.UpdateAll();
	BulletsBuffer.UpdateAll();
    
    //draw
    glUseProgram(Shader.ProgramId);
	glUniform1i(Shader.MainTextureId, 0);
	glUniform1i(Shader.LightTextureId, 1);
	glUniform2f(Shader.ResolutionId, WindowWidth, WindowHeight);
	glUniform4fv(Shader.AmbientColorId, 1, &whiteColor[0]);
	glUniformMatrix4fv(Shader.MVPId, 1, GL_FALSE, Scene.GetDynamicMVP());
	PlayersBuffer.Begin();
		LevelsBuffers[LevelNumber]->Draw();
		PlayersBuffer.DrawAll();
		BulletsBuffer.DrawAll();
	PlayersBuffer.End();
}

MGraphicClient::MGraphicClient()
{
	txWall = NULL;
	txPlayer = NULL;
	txPlayerSpawn = NULL;
	txAmmoSpawn = NULL;
	txBullet = NULL;
	window = NULL;
	WindowWidth = 800;
	WindowHeight = 600;
	memset(keys, 0, 1024);
	mx = 0;
	my = 0;
	MouseSceneCoord = glm::vec2(0, 0);
	MainDirection = glm::vec2(0, 0);
	MainVelocity = glm::vec2(0, 0);
	MainCenter = glm::vec2(0, 0);
	MainRotation = 0;
	whiteColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	LevelNumber = -1;
	NeedUpdateBulletsBuffer = false;
	NeedCloseWindow = false;
}

MGraphicClient::~MGraphicClient()
{
	txWall = NULL;
	txPlayer = NULL;
	txPlayerSpawn = NULL;
	txAmmoSpawn = NULL;
	txBullet = NULL;
	window = NULL;
	WindowWidth = 800;
	WindowHeight = 600;
	memset(keys, 0, 1024);
	mx = 0;
	my = 0;
	MouseSceneCoord = glm::vec2(0, 0);
	MainDirection = glm::vec2(0, 0);
	MainVelocity = glm::vec2(0, 0);
	MainCenter = glm::vec2(0, 0);
	MainRotation = 0;
	whiteColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

bool MGraphicClient::Initialize()
{
    srand(time(NULL));
    LogFile<<"Randomized"<<endl;
	  
    glfwSetErrorCallback(WindowErrorCallback);   
    if (!glfwInit()) return -1;
    window = glfwCreateWindow(WindowWidth, WindowHeight, "TestApp", NULL, NULL); //how create hidden by default
    if(!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, WindowKeyboardCallback);
    glfwSetCursorPosCallback(window, WindowMouseCallback);
    glfwSetMouseButtonCallback (window, WindowMouseClickCallback);
    glfwMakeContextCurrent(window);
    if(glfwExtensionSupported("WGL_EXT_swap_control")) glfwSwapInterval(1);//0 - disable, 1 - enable
    LogFile<<"Window created: width: "<<WindowWidth<<" height: "<<WindowHeight<<endl;

	//glew
	GLenum Error = glewInit();
	if(GLEW_OK != Error)
	{
		LogFile<<"Window: GLEW Loader error: "<<glewGetErrorString(Error)<<endl;
		return 0;
	}
	LogFile<<"GLEW initialized"<<endl;
	
	if(!IsOpenglSupported()) return false;

	//shaders
	if(!Shader.CreateShaderProgram("shaders/main2.vertexshader.glsl", "shaders/main2.fragmentshader.glsl")) return false;
	if(!Shader.PrepareShaderValues()) return false;
	LogFile<<"Shaders loaded"<<endl;

	//load textures
	unsigned int CountLoaded;
	if(!(txPlayer = TextureLoader.LoadTexture("textures/tex01.png", 1, 1, 0, CountLoaded, GL_NEAREST, GL_REPEAT, false))) return false;
	if(!(txBullet = TextureLoader.LoadTexture("textures/tex02.png", 1, 1, 0, CountLoaded, GL_NEAREST, GL_REPEAT, false))) return false;
	if(!(txWall = TextureLoader.LoadTexture("textures/tex03.png", 1, 1, 0, CountLoaded, GL_NEAREST, GL_REPEAT, false))) return false;
	if(!(txPlayerSpawn = TextureLoader.LoadTexture("textures/tex04.png", 1, 1, 0, CountLoaded, GL_NEAREST, GL_REPEAT, false))) return false;
	if(!(txAmmoSpawn = TextureLoader.LoadTexture("textures/tex05.png", 1, 1, 0, CountLoaded, GL_NEAREST, GL_REPEAT, false))) return false;
	LogFile<<"Textures loaded"<<endl;
	
	//scene
	if(!Scene.Initialize(&WindowWidth, &WindowHeight)) return false;
	LogFile<<"Scene initialized"<<endl;
	
	//init levels buffers
	stQuad Quad;
	Level.Initialize();
	SetQuad(Quad, 0,0,1,1);
	for(int i=0; i<Level.LevelsData.size(); i++)
	{
		LevelsBuffers.insert(pair<unsigned char, MStaticBuffer*>(i, new MStaticBuffer));
		LevelsBuffers[i]->Initialize(txWall);
		for(int j=0; j<Level.LevelsData[i].Walls.size(); j++)
			LevelsBuffers[i]->AddUVQuad(Level.LevelsData[i].Walls[j], Quad);
		LevelsBuffers[i]->Dispose();
	}
	
	//init players buffer
	if(!PlayersBuffer.Initialize(GL_STREAM_DRAW)) return false;
	for(unsigned char i=0; i<2; i++)
	{
		PlayersObjects.insert(pair<unsigned char, MObject*>(i, new MObject));
		if(!PlayersBuffer.AddObject(PlayersObjects[i], 0.32 + i * 0.64, 0.32 + i * 0.64, 0.64, 0.64, 0, 0, 1, 1, txPlayer->Id)) return false;
	}
	if(!PlayersBuffer.DisposeAll()) return false;
	
	if(!BulletsBuffer.Initialize(GL_STREAM_DRAW)) return false;
	if(!BulletsBuffer.DisposeAll()) return false;
	LogFile<<"Buffers initialized"<<endl;
	
	return true;
}

void MGraphicClient::Close()
{
	LogFile<<"Closing application. Free memory"<<endl;
	
	for(map<unsigned char, MStaticBuffer*>::iterator it = LevelsBuffers.begin(); it != LevelsBuffers.end(); ++it)
	{
		it->second->Close();
		delete it->second;
	}
	LevelsBuffers.clear();
	
	PlayersBuffer.Close();
	for(map<unsigned char, MObject*>::iterator it = PlayersObjects.begin(); it != PlayersObjects.end(); ++it)
	{
		delete it->second;
	}
	
	BulletsBuffer.Close();
	for(map<unsigned char, MObject*>::iterator it = BulletsObjects.begin(); it != BulletsObjects.end(); ++it)
	{
		delete it->second;
	}
	
	PlayersObjects.clear();
	BulletsObjects.clear();
	BulletsToAdd.clear();
	
	TextureLoader.DeleteTexture(txPlayer, ONE);
	TextureLoader.DeleteTexture(txBullet, ONE);
	TextureLoader.DeleteTexture(txWall, ONE);
	TextureLoader.DeleteTexture(txPlayerSpawn, ONE);
	TextureLoader.DeleteTexture(txAmmoSpawn, ONE);
	TextureLoader.Close();
	Shader.Close();
	
	glfwTerminate();
}
