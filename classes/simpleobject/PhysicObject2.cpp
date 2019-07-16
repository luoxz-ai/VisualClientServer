#include "../../stdafx.h"
#include "PhysicObject2.h"

MPhysicObject2::MPhysicObject2():MObject()
{
	Velocity = glm::vec2(0, 0);
	IsBullet = false;
	NullQuad(CollisionBox);
}

MPhysicObject2::~MPhysicObject2()
{
	Velocity = glm::vec2(0, 0);
	IsBullet = false;
	NullQuad(CollisionBox);
}

void MPhysicObject2::SetIsBullet(bool inIsBullet)
{
	IsBullet = inIsBullet;
}

bool MPhysicObject2::GetIsBullet()
{
	return IsBullet;
}

bool MPhysicObject2::AddCollideObject(MPhysicObject2* pPhysicObject2)
{
	if(!pPhysicObject2) return false;
	it = find(CollideObjects.begin(), CollideObjects.end(), pPhysicObject2);
	if(it != CollideObjects.end()) return false;
	CollideObjects.push_back(pPhysicObject2);
	return true;
}

bool MPhysicObject2::RemoveCollideObject(MPhysicObject2* pPhysicObject2)
{
	if(!pPhysicObject2) return false;
	it = find(CollideObjects.begin(), CollideObjects.end(), pPhysicObject2);
	if(it == CollideObjects.end()) return false;
	CollideObjects.erase(it);
	return true;
}

void MPhysicObject2::SetVelocity(glm::vec2 inVelocity)
{
	Velocity = inVelocity;
}

glm::vec2 MPhysicObject2::GetVelocity()
{
	return Velocity;
}

int MPhysicObject2::GetCollisionsCount()
{
	return CollideObjects.size();
}

void MPhysicObject2::InitCollisionBox()
{
	CollisionBox = GetVertex();
}

stQuad MPhysicObject2::GetCollisionBox()
{
	return CollisionBox;
}

void MPhysicObject2::Move(glm::vec2 Velocity)
{
	MoveVertex(Velocity);
	MoveQuad(CollisionBox, Velocity);
}

void MPhysicObject2::OnBeginCollide(MPhysicObject2* pPhysicObject2)
{
}

void MPhysicObject2::OnEndCollide(MPhysicObject2* pPhysicObject2)
{
}
