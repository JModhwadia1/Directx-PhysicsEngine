#pragma once
#include "Transform.h"

class SphereCollider;
class BoxCollider;

struct ContactPoint
{
	Vector3 Position;
	float penetrationDepth;
};

struct CollisionManifold
{
	int contactPointCount;
	ContactPoint points[4];
	Vector3 collisionNormal;
};


class Collider abstract
{
protected:
	Transform* _transform;
public:
	Collider(Transform* transform) { _transform = transform; }

	virtual bool CollidesWith(Collider& other, CollisionManifold& out) = 0;
	virtual bool CollidesWith(SphereCollider& other, CollisionManifold& out) = 0;
	virtual bool CollidesWith(BoxCollider& other, CollisionManifold& out) = 0;

	Vector3 GetPosition() const { return _transform->GetPosition(); }

	
};