#include "Collider.h"
#include "BoxCollider.h"

inline float DistanceToPointAABB(Vector3 point, BoxCollider& coll)
{
    float sqDist = 0.0f;
    
    // For each axis count any excess distance outside box extents
    float v = point.x;
    // X axis test
    if (v < coll.GetMin().x) sqDist += (coll.GetMin().x - v) * (coll.GetMin().x - v);
    if (v > coll.GetMax().x) sqDist += (v - coll.GetMax().x) * (v - coll.GetMax().x);
    // Y axis test
    if (v < coll.GetMin().y) sqDist += (coll.GetMin().y - v) * (coll.GetMin().y - v);
    if (v > coll.GetMax().y) sqDist += (v - coll.GetMax().y) * (v - coll.GetMax().y);
    // Z axis test
    if (v < coll.GetMin().z) sqDist += (coll.GetMin().z - v) * (coll.GetMin().z - v);
    if (v > coll.GetMax().z) sqDist += (v - coll.GetMax().z) * (v - coll.GetMax().z);

    return sqDist;
}
