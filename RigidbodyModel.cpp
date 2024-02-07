#include "RigidbodyModel.h"

RigidbodyModel::RigidbodyModel(Transform* transform, float mass) : PhysicsModel(transform, mass)
{	
	// Set inertia tensor
	_inertiaTensor._11 = (_mass * ((1 * 1) + (1 * 1))) / 12;
	_inertiaTensor._12 = 0;
	_inertiaTensor._13 = 0;

	_inertiaTensor._21 = 0;
	_inertiaTensor._22 = (_mass * ((1 * 1) + (1 * 1))) / 12;
	_inertiaTensor._23 = 0;

	_inertiaTensor._31 = 0;
	_inertiaTensor._32 = 0;
	_inertiaTensor._33 = (_mass * ((1 * 1) + (1 * 1))) / 12;
	// Init Angular Velocity to 0,0,0
	_angularVelocity = { 0,0,0 };
}

void RigidbodyModel::Update(float deltaTime)
{
	// Call base update
	PhysicsModel::Update(deltaTime);
	// Calculate Angular Velocity
	CalculateAngularVelocity(deltaTime);
	// Calculate Current Orientation
	CalculateCurrentOrientation(deltaTime);
	// Set Torque back to 0,0,0
	_torque = { 0,0,0 };
	
}

void RigidbodyModel::AddRelativeForce(Vector3 force, Vector3 point)
{
	// Torque = relative position X force
	// X = Cross Product
	 _torque = point ^ force;
	 // Add torque
	 AddForce(_torque);
}

Vector3 RigidbodyModel::CalculateAngularVelocity(float deltaTime)
{
	if (_mass == 0)
	{
		return Vector3();
	}
	// Conver torque into a XMVECTOR
	XMVECTOR torqueVec = { _torque.x, _torque.y, _torque.z };
	// Calculate angular accel using the converted torque vector * the inverse of the intertia matrix
	XMVECTOR angularAcceleration = XMVector3Transform(torqueVec, XMMatrixInverse(nullptr, XMLoadFloat3x3(&_inertiaTensor)));
	// Angular Velocity += Angular Acceleration * deltaTime 
	_angularVelocity += Vector3(XMVectorGetX(angularAcceleration), XMVectorGetY(angularAcceleration), XMVectorGetZ(angularAcceleration)) * deltaTime;
	
	// Modify the orientatino using a defferential
	//_transform->GetOrientation() += _transform->GetOrientation() * _angularVelocity * 0.5f * deltaTime;

	// Return the modified Angular Velocity
	return _angularVelocity;
	
}

void RigidbodyModel::CalculateCurrentOrientation(float deltaTime)
{
	// Get the orientation
	Quaternion Orientation = _transform->GetOrientation();

	// Modify orientation using the defferiential
	Orientation += Orientation * _angularVelocity * 0.5f * deltaTime;

	if (Orientation.Magnitude() != 0)
	{
		//Orientation.Magnitude() / Orientation.Magnitude();
		Orientation /= Orientation.Magnitude();
	}

	// If the magnitude of the orientation is not zero, divide by its own magnitude to normalize
	// Set the updated orientation
	_transform->SetOrientation(Orientation);
	// Dampen velocity for angular drag
	 _angularVelocity *= pow(_angularDamping, deltaTime);
}
