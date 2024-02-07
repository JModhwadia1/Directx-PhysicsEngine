#include "SphereCollider.h"

bool SphereCollider::CollidesWith(SphereCollider& other, CollisionManifold& out)
{
	//Vector3 distanceBetweenObjects;
	//distanceBetweenObjects = other.GetPosition() - this->GetPosition();

	//float combinedRadius;
	//combinedRadius = this->GetRadius() + other.GetRadius();

	//float distX = this->GetPosition().x - other.GetPosition().x;
	//float distY = this->GetPosition().y - other.GetPosition().y;
	//float distZ = this->GetPosition().z - other.GetPosition().z;

	//float distToObject = sqrt(distX * distX + distY * distY + distZ * distZ);
	//
	///*if (distanceBetweenObjects.x < combinedRadius && distanceBetweenObjects.y < combinedRadius && distanceBetweenObjects.z < combinedRadius)
	//{
	//	return true;
	//}
	//else
	//{
	//	return false;
	//}*/
	//if (distToObject <= (this->GetRadius() + other.GetRadius()))
	//{
	//	return true;
	//}
	//else
	//{
	//	return false;
	//}

	// Using collision manifold
	// Get the difference between objects
	Vector3 diff = other.GetPosition() - GetPosition();
	// Get the Radii sum of the two objects
	float radiiSum = other.GetRadius() + GetRadius();
	// Check to see if we are colliding
	if (diff.Magnitude() < radiiSum)
	{
		// Set the collision normal
		out.collisionNormal = diff;
		// Normalise the collision normal
		out.collisionNormal.Normalize();
		// Populate the contact point list to 1 in the collision manifold
		out.contactPointCount = 1;
		// Set the position in the collision manifold
		out.points[0].Position = GetPosition() + (out.collisionNormal * GetRadius());
		// Set the penetration depth in the collision manifold
		out.points[0].penetrationDepth = fabs(diff.Magnitude() - radiiSum);

		return true;		
	}
	return false;
}

bool SphereCollider::CollidesWith(BoxCollider& other, CollisionManifold& out)
{
	return false;
}
