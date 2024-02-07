#include "ParticleModel.h"

ParticleModel::ParticleModel(Transform* tf, float resetTime, Vector3 pertubation, bool invertGravity)
{
	_transform = tf;
	_startingPosition = tf->GetPosition();
	_resetTime = resetTime;
}

void ParticleModel::Update(float deltaTime)
{
	_timeAlive += deltaTime;
	
	//Perturb velocity
	//noiseX = perlinNoise(position.x, 0, 0)
	//noiseY = perlinNoise(0, position.y, 0)
	//...
	// 
	//velocity.x = noiseX * perturbation.x;
	//velocity.y = ...
	//..


	if (_timeAlive >= _resetTime)
	{
		Reset();
	}

	PhysicsModel::Update(deltaTime);
}

void ParticleModel::AddRelativeForce(Vector3 force, Vector3 point)
{
	AddForce(force);
}

void ParticleModel::SetMass(float mass)
{
	_mass = mass;
}

void ParticleModel::Reset()
{
	_timeAlive = 0.0f;
	_transform->SetPosition(_startingPosition);
}
