#include "../../stdafx.h"
#include "PhysicWorld2.h"

MPhysicWorld2::MPhysicWorld2()
{
	Gravity = glm::vec2(0, 0);
}

MPhysicWorld2::~MPhysicWorld2()
{
	Gravity = glm::vec2(0, 0);
}

bool MPhysicWorld2::AddDynamicObject(MPhysicObject2* pObject)
{
	if(!pObject) return false;
	it = find(DynamicObjects.begin(), DynamicObjects.end(), pObject);
	if(it != DynamicObjects.end()) return false;
	pObject->InitCollisionBox();
	DynamicObjects.push_back(pObject);
	return true;
}

bool MPhysicWorld2::AddStaticObject(MPhysicObject2* pObject)
{
	if(!pObject) return false;
	it = find(StaticObjects.begin(), StaticObjects.end(), pObject);
	if(it != StaticObjects.end()) return false;
	pObject->InitCollisionBox();
	StaticObjects.push_back(pObject);
	return true;
}

bool MPhysicWorld2::RemoveStaticObject(MPhysicObject2* pObject)
{
	if(!pObject) return false;
	it = find(StaticObjectsToRemove.begin(), StaticObjectsToRemove.end(), pObject);
	if(it == StaticObjectsToRemove.end()) StaticObjectsToRemove.push_back(pObject);
	return true;
}

bool MPhysicWorld2::RemoveDynamicObject(MPhysicObject2* pObject)
{
	if(!pObject) return false;
	it = find(DynamicObjectsToRemove.begin(), DynamicObjectsToRemove.end(), pObject);
	if(it == DynamicObjectsToRemove.end()) DynamicObjectsToRemove.push_back(pObject);
	return true;
}

bool MPhysicWorld2::RemoveStaticObjectForce(MPhysicObject2* pObject)
{
	if(!pObject) return false;
	it = find(StaticObjects.begin(), StaticObjects.end(), pObject);
	if(it == StaticObjects.end()) return false;
	StaticObjects.erase(it);
	return true;
}

bool MPhysicWorld2::RemoveDynamicObjectForce(MPhysicObject2* pObject)
{
	if(!pObject) return false;
	it = find(DynamicObjects.begin(), DynamicObjects.end(), pObject);
	if(it == DynamicObjects.end()) return false;
	DynamicObjects.erase(it);
	return true;
}

void MPhysicWorld2::SetGravity(glm::vec2 inGravity)
{
	Gravity = inGravity;
}

void MPhysicWorld2::Step()
{
	if(!DynamicObjects.size()) return;
	
	//all dynamic objects cycle
	for(int j=0; j<DynamicObjects.size(); j++)
	{
		Velocity = DynamicObjects[j]->GetVelocity();
		if(!Velocity.x && !Velocity.y) continue;
	
		//get quads that can be collided	
		CurrentQuad = DynamicObjects[j]->GetCollisionBox();
		NextQuad = CurrentQuad;
		MoveQuad(NextQuad, Velocity);
		MergeQuads(TestQuad, CurrentQuad, NextQuad);
		CollisionObjects.clear();
		for(int i=0; i<StaticObjects.size(); i++)
		{
			if(QuadQuadCollision(TestQuad, StaticObjects[i]->GetCollisionBox())) CollisionObjects.push_back(StaticObjects[i]);
		}
		if(!CollisionObjects.size())
		{
			DynamicObjects[j]->Move(Velocity);
			continue;
		}
		
		//sort by nearest distance ?
		sort(CollisionObjects.begin(), CollisionObjects.end(), stNearestPhysicObject(DynamicObjects[j]));
		
		//collide test with velocity projections
		for(int i=0; i<CollisionObjects.size(); i++)
		{
			if(!Velocity.x && !Velocity.y) break;
			if(QuadQuadIntersect(CollisionObjects[i]->GetCollisionBox(), CurrentQuad, Velocity, IntersectPoint))
			{
				//if(DynamicObjects[j]->AddCollideObject(CollisionObjects[i])) DynamicObjects[j]->OnBeginCollide(CollisionObjects[i]);//ERROR!
				Velocity.x *= !QuadQuadIntersect(CollisionObjects[i]->GetCollisionBox(), CurrentQuad, glm::vec2(Velocity.x, 0), IntersectPoint);
				Velocity.y *= !QuadQuadIntersect(CollisionObjects[i]->GetCollisionBox(), CurrentQuad, glm::vec2(0, Velocity.y), IntersectPoint);
			}
			//else if(DynamicObjects[j]->RemoveCollideObject(CollisionObjects[i])) DynamicObjects[j]->OnEndCollide(CollisionObjects[i]);//ERROR!
		}
		DynamicObjects[j]->Move(Velocity);
	}
}

void MPhysicWorld2::RemoveObjects()
{
	for(int i=0; i<DynamicObjectsToRemove.size(); i++)
	{
		it = find(DynamicObjects.begin(), DynamicObjects.end(), DynamicObjectsToRemove[i]);
		if(it != DynamicObjects.end()) DynamicObjects.erase(it);
	}
	for(int i=0; i<StaticObjectsToRemove.size(); i++)
	{
		it = find(StaticObjects.begin(), StaticObjects.end(), StaticObjectsToRemove[i]);
		if(it != StaticObjects.end()) StaticObjects.erase(it);
	}
	DynamicObjectsToRemove.clear();
	StaticObjectsToRemove.clear();
}

void MPhysicWorld2::Close()
{
	DynamicObjects.clear();
	StaticObjects.clear();
	DynamicObjectsToRemove.clear();
	StaticObjectsToRemove.clear();
}
