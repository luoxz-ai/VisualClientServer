#ifndef physicworld2H
#define physicworld2H

#include "PhysicObject2.h"

class MPhysicWorld2
{
private:
	glm::vec2 Gravity;
	vector<MPhysicObject2*> DynamicObjects;
	vector<MPhysicObject2*> StaticObjects;
	vector<MPhysicObject2*>::iterator it;
	
	vector<MPhysicObject2*> DynamicObjectsToRemove;
	vector<MPhysicObject2*> StaticObjectsToRemove;
	
	stQuad CurrentQuad;
	stQuad NextQuad;
	stQuad TestQuad;
	glm::vec2 IntersectPoint;
	glm::vec2 Velocity;
	vector<MPhysicObject2*> CollisionObjects;
public:
	MPhysicWorld2();
	~MPhysicWorld2();
	bool AddDynamicObject(MPhysicObject2* pObject);
	bool AddStaticObject(MPhysicObject2* pObject);
	bool RemoveStaticObject(MPhysicObject2* pObject);
	bool RemoveDynamicObject(MPhysicObject2* pObject);
	bool RemoveStaticObjectForce(MPhysicObject2* pObject);
	bool RemoveDynamicObjectForce(MPhysicObject2* pObject);
	void SetGravity(glm::vec2 inGravity);
	void Step();
	void RemoveObjects();
	void Close();
};

#endif
