#include "../../stdafx.h"
#include "Scene.h"

MScene::MScene()
{
	pWindowWidth = NULL;
	pWindowHeight = NULL;
	
	CameraRound = 100;//1000
	CoordinateScale = 0.01; //1 metre - 100 pixels
	ViewScale = 1.0f;
	ViewOffset = glm::vec2(0.0f, 0.0f);
	ViewCenter = glm::vec2(0.0f, 0.0f);
	VectorZ = glm::vec2(-5.0f, 5.0f);
	//new
	FollowVelocity = 0.01f;//best step
}

MScene::~MScene()
{
	pWindowWidth = NULL;
	pWindowHeight = NULL;
}

bool MScene::Initialize(int* inpWindowWidth, int* inpWindowHeight)
{
	if(!inpWindowWidth || !inpWindowHeight)
	{
		LogFile<<"Scene: null window size"<<endl;
		return false;
	}
	
	if(*inpWindowWidth < MIN_WINDOW_SIZE_X || *inpWindowHeight < MIN_WINDOW_SIZE_Y)
	{
		LogFile<<"Scene: wrong window size"<<endl;
		return false;
	}

	pWindowWidth = inpWindowWidth;
	pWindowHeight = inpWindowHeight;

	//prepare view
	Projection = glm::ortho(0.0f, (float)*pWindowWidth * CoordinateScale, 0.0f, (float)*pWindowHeight * CoordinateScale, VectorZ[0], VectorZ[1]);
	CameraPosition = glm::vec3(0.0f, 0.0f, 1.0f);
	CameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
	Model = glm::mat4(1.0f);
	View = glm::lookAt(CameraPosition, CameraDirection, glm::vec3(0.0f, 1.0f, 0.0f));
    StaticMVP = DynamicMVP = Projection * View * Model;
    
    //view center
    ViewCenter = glm::vec2((*pWindowWidth >> 1) * CoordinateScale, (*pWindowHeight >> 1) * CoordinateScale);
    glViewport(0, 0, *pWindowWidth, *pWindowHeight);//IMPORTANT!
	
	return true;
}

void MScene::CalculateMVP()
{
	View = glm::lookAt(CameraPosition, CameraDirection, glm::vec3(0.0f, 1.0f, 0.0f));
	DynamicMVP = Projection * View * Model;
}

void MScene::MoveCamera(glm::vec2 Delta)
{
	CameraPosition.x += Delta.x;
	CameraPosition.y += Delta.y;//round(Delta.y * 1000) / 1000;
	CameraDirection.x = CameraPosition.x;
	CameraDirection.y = CameraPosition.y;
	//cout<<CameraPosition.x<<" "<<CameraPosition.y<<endl;
}

GLfloat* MScene::GetDynamicMVP()
{
	return (GLfloat*)&DynamicMVP;
}

GLfloat* MScene::GetStaticMVP()
{
	return (GLfloat*)&StaticMVP;
}

void MScene::SetViewScale(float inViewScale)
{
	ViewScale = inViewScale;
	ViewCenter = glm::vec2((*pWindowWidth * 0.5f) * (1 + (1.0f - ViewScale)), (*pWindowHeight * 0.5f) * (1 + (1.0f - ViewScale)));
}

void MScene::Size()
{
	if(!pWindowWidth || !pWindowHeight) return;
	cout<<"Size function: "<<*pWindowWidth<<" "<<*pWindowHeight<<endl;
	
	SetViewScale(ViewScale);
	
	//change projection matix and default MVP
	Projection = glm::ortho(0.0f, (float)*pWindowWidth * CoordinateScale, 0.0f, (float)*pWindowHeight * CoordinateScale, VectorZ[0], VectorZ[1]);
    StaticMVP = Model * Projection * glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    //change viewport
	ViewCenter = glm::vec2((*pWindowWidth >> 1) * CoordinateScale, (*pWindowHeight >> 1) * CoordinateScale);
	glViewport(0, 0, *pWindowWidth, *pWindowHeight);//IMPORTANT!
}

void MScene::ViewAt(glm::vec2 Point)
{
	//CameraPosition.x = roundf((CameraDirection.x = ViewOffset.x + Point.x - ViewCenter.x) * 100) / 100;
	//CameraPosition.y = roundf((CameraDirection.y = ViewOffset.y + Point.y - ViewCenter.y) * 100) / 100;
	CameraPosition.x = CameraDirection.x = ViewOffset.x + Point.x - ViewCenter.x;
	CameraPosition.y = CameraDirection.y = ViewOffset.y + Point.y - ViewCenter.y;
	View = glm::lookAt(CameraPosition, CameraDirection, glm::vec3(0, 1, 0));
    DynamicMVP = Projection * View * Model;
}

glm::vec2 MScene::WindowPosToWorldPos(double PosX, double PosY)
{	
	if(PosX < 0 || PosY < 0) return glm::vec2(0, 0);
	glm::vec2 RetValue = glm::vec2(PosX, (double)*pWindowHeight - PosY) * CoordinateScale + (glm::vec2)CameraPosition;
	return RetValue;
}
//new
bool MScene::SetTarget(MObject* inpTarget)
{
	if(!inpTarget) return false;
	pTarget = inpTarget;
	return true;
}

void MScene::Follow(float Delta)
{
	if(!pTarget) return;
	glm::vec2 CameraCenterView = (glm::vec2)CameraPosition + ViewCenter;
	float Distance = glm::distance(GetQuadCenter(pTarget->GetVertex()), CameraCenterView);
	if(Distance < 0.01f) return;
	glm::vec2 Direction = glm::normalize(GetQuadCenter(pTarget->GetVertex()) - CameraCenterView);
	glm::vec2 Sign = glm::vec2(sgn(Direction.x), sgn(Direction.y));
	CameraPosition.x += Sign.x * FollowVelocity;
	CameraPosition.y += Sign.y * FollowVelocity;
	//CameraPosition.x += floor(Direction.x * FollowVelocity * Delta * 1000) / 1000;
	//CameraPosition.y += floor(Direction.y * FollowVelocity * Delta * 1000) / 1000;
	//CameraPosition.x += Direction.x * Distance * Delta;
	//CameraPosition.y += Direction.y * Distance * Delta;
	//CameraPosition.x += Direction.x * FollowVelocity * Delta;
	//CameraPosition.y += Direction.y * FollowVelocity * Delta;
	///CameraPosition.x = floor(CameraPosition.x * 1000) / 1000;
	//CameraPosition.y = floor(CameraPosition.y * 1000) / 1000;
	//cout<<CameraPosition.x<<" "<<CameraPosition.y<<" "<<Distance<<endl;
	CameraDirection.x = CameraPosition.x;
	CameraDirection.y = CameraPosition.y;
}
