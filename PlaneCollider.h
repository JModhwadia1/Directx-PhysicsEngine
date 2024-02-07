#pragma once
#include "Collider.h"
#include "BoxCollider.h"
class PlaneCollider : public Collider
{
public:
	PlaneCollider(Transform* transform, Vector3 normal) : Collider(transform) { _normal = normal; }

	virtual bool CollidesWith(Collider& other, CollisionManifold& out) override { return other.CollidesWith(*this, out); }
	virtual bool CollidesWith(SphereCollider& other, CollisionManifold& out) override;
	virtual bool CollidesWith(BoxCollider& other, CollisionManifold& out) override;

private:
	Vector3 _normal;

};

