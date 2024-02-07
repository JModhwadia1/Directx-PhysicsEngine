#pragma once
#include "Collider.h"

class SphereCollider : public Collider
{
public:
	SphereCollider(Transform* transform, float radius) : Collider(transform) { _radius = radius; }

	virtual bool CollidesWith(Collider& other, CollisionManifold& out) override { return other.CollidesWith(*this, out); }
	virtual bool CollidesWith(SphereCollider& other, CollisionManifold& out) override;
	virtual bool CollidesWith(BoxCollider& other, CollisionManifold& out) override;

	float GetRadius() const { return _radius; }
private:
	float _radius = 1.0f;
};

