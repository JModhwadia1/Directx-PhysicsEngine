#pragma once
#include "Collider.h"
#include "Debug.h"

class BoxCollider : public Collider
{
public:
	BoxCollider(Transform* transform, Vector3 halfextents) : Collider(transform) { _halfExtents = halfextents; }

	virtual bool CollidesWith(Collider& other, CollisionManifold& out) override { return other.CollidesWith(*this, out); }
	virtual bool CollidesWith(SphereCollider& other, CollisionManifold& out) override;
	virtual bool CollidesWith(BoxCollider& other, CollisionManifold& out) override;

	Vector3 GetCenter() { return _center; }
	void  SetCenter(Vector3 center) { _center = center; }

	Vector3 GetMin() { return _min; }
	Vector3 GetMax() { return _max; }


	Vector3 GetHalfExtents() { return _halfExtents; }

private:
	Vector3 _center;
	Vector3 _halfExtents;
	Vector3 _max;
	Vector3 _min;

};

