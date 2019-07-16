#ifndef physicobject2H
#define physicobject2H

#include "Object.h"

class MPhysicObject2: public MObject
{
protected:
	stQuad CollisionBox;//collision box
	vector<MPhysicObject2*> CollideObjects;
	vector<MPhysicObject2*>::iterator it;
	glm::vec2 Velocity;
	bool IsBullet;
public:
	MPhysicObject2();
	~MPhysicObject2();
	void SetIsBullet(bool inIsBullet);
	bool GetIsBullet();
	bool AddCollideObject(MPhysicObject2* pPhysicObject2);
	bool RemoveCollideObject(MPhysicObject2* pPhysicObject2);
	void SetVelocity(glm::vec2 inVelocity);
	glm::vec2 GetVelocity();
	int GetCollisionsCount();
	void InitCollisionBox();
	stQuad GetCollisionBox();
	void Move(glm::vec2 Velocity);
	virtual void OnBeginCollide(MPhysicObject2* pPhysicObject2);
	virtual void OnEndCollide(MPhysicObject2* pPhysicObject2);
};

struct stNearestPhysicObject
{
	MPhysicObject2* pPhysicObject2;
	stNearestPhysicObject(MPhysicObject2* inpPhysicObject2)
	{
		pPhysicObject2 = inpPhysicObject2;
	}
	bool operator () (MPhysicObject2* pObject1, MPhysicObject2* pObject2)
	{
		float Distance1, Distance2;
		Distance1 = glm::distance(GetQuadCenter(pPhysicObject2->GetVertex()), GetQuadCenter(pObject1->GetVertex()));
		Distance2 = glm::distance(GetQuadCenter(pPhysicObject2->GetVertex()), GetQuadCenter(pObject2->GetVertex()));
		return Distance1 < Distance2;
	}
};

#endif
