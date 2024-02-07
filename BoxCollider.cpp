#include "BoxCollider.h"
#include "Collider.cpp"

bool BoxCollider::CollidesWith(SphereCollider& other, CollisionManifold& out)
{
	return false;
}
bool BoxCollider::CollidesWith(BoxCollider& other, CollisionManifold& out)
{
	// Get the center of this collider
	_center = this->GetPosition();
	// Get the max of this collider
	_max = _center + _halfExtents;
	// Get the min of this collider
	_min = _center - _halfExtents;

	// Get the Min,Max and Center of the other collider
	Vector3 theirMax, theirMin, theirCenter;
	theirCenter = other.GetPosition();
	theirMax = theirCenter + other.GetHalfExtents();
	theirMin = theirCenter - other.GetHalfExtents();

	// Check to see if there is collision
	if (this->_max.x < theirMin.x || this->_min.x > theirMax.x)
		return false;
	if (this->_max.y < theirMin.y || this->_min.y > theirMax.y)
		return false;
	if (this->_max.z < theirMin.z || this->_min.z > theirMax.z)
		return false;

	out.contactPointCount = 1;
	out.collisionNormal = other.GetPosition() - GetPosition();
	out.collisionNormal.Normalize();
	out.points[0].Position = GetPosition() + out.collisionNormal;
	out.points[0].penetrationDepth = DistanceToPointAABB(out.points[0].Position, *this);

	return true;
	
}

