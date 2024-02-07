#pragma once
#include "PhysicsModel.h"

class RigidbodyModel : public PhysicsModel
{
public:
	RigidbodyModel(Transform* transform, float mass) /*: PhysicsModel(transform, mass) {};*/;


	void Update(float deltaTime) override;
	void AddRelativeForce(Vector3 force, Vector3 point) override;
	Vector3 CalculateAngularVelocity(float deltaTime);
	void CalculateCurrentOrientation(float deltaTime);
	void SetMass(float mass) { _mass = mass; }
private:
	Vector3 _angularVelocity;
	Vector3 _torque;

	XMFLOAT3X3 _inertiaTensor;
	float _angularDamping = 0.99f;

};


