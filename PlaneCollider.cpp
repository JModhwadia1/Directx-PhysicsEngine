#include "PlaneCollider.h"


bool PlaneCollider::CollidesWith(SphereCollider& other, CollisionManifold& out)
{
    return false;
}

bool PlaneCollider::CollidesWith(BoxCollider& other, CollisionManifold& out)
{
    if(other.GetMax().y > this->GetPosition().y && other.GetMin().y < this->GetPosition().y)
    {
        return true;
    }
}
