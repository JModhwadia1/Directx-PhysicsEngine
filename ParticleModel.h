#pragma once
#include "PhysicsModel.h"
class ParticleModel : public PhysicsModel
{
public:
	ParticleModel(Transform* transform, float mass) : PhysicsModel(transform, mass) {};

	// Particle model specific constructor
	// Start Position, Time to reset afterwards, Randomness associated with movement, gravity inversion - for e.g. smoke particles
	ParticleModel(Transform* tf, float resetTime, Vector3 pertubation, bool invertGravity);

	void Update(float deltaTime) override;
	void AddRelativeForce(Vector3 force, Vector3 point) override;
	void SetMass(float mass) override;
	void Reset();
private:
	Vector3 _startingPosition;
	float _timeAlive = 0.0f;
	float _resetTime;
};

