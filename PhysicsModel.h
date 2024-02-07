#pragma once
#include "Transform.h"
#include "Debug.h"

///
///	Abstract Class
/// 
/// 
class Collider;

class PhysicsModel
{
public:

	PhysicsModel();
	PhysicsModel(Transform* transform, float mass);

	virtual void Update(float deltaTime) = 0;
	virtual void AddRelativeForce(Vector3 force, Vector3 point) = 0;

	// Getters and Setters
	void SetVelocity(Vector3 velocity) { _velocity = velocity; }
	Vector3 GetVelocity() { return _velocity; }

	void SetAcceleration(Vector3 acceleration){_acceleration = acceleration;}
	Vector3 GetAcceleration() { return _acceleration; }

	void SetUseAcceleration(bool accelerataion) { _useAcceleration = accelerataion; }
	bool GetUseAcceleration() { return _useAcceleration; }
	
	void SetUseGravity(bool gravity) { _useGravity = gravity; }

	// Used to add force each frame
	void AddForce(Vector3 force) { _netForce += force; }

	Vector3 GravityForce() { return _gravity * _mass; }

	float GetMass() { return _mass; }
	virtual void SetMass(float mass) = 0;
	float GetInverseMass() { if (_mass == 0) return 0; return 1.0f / _mass; }
	Vector3 CalculateDragForce();
	Vector3 CalculateFrictionForce();
	Vector3 CalculateLiftForce();
	Vector3 CalculateThrustForce(float deltaTime);
	Vector3 CalculateWindForce();

	bool GetUseLift() { return _useLift;}
	void SetUseLift(bool lift) { _useLift = lift; }

	bool GetUseThrust() { return _useThrust;}
	void SetUseThrust(bool thrust) {_useThrust = thrust; }

	bool GetUseDrag() { return _useDrag; }
	void SetUseDrag(bool drag) { _useDrag = drag; }

	bool GetUseFriction() { return _useFriction; }
	void SetUseFriction(bool friction) { _useFriction = friction; }
	// Collision
	bool IsCollideable() const { return _collider != nullptr; }
	Collider* GetCollider() const { return _collider; }
	void SetCollider(Collider* collider) { _collider = collider; }
	// Collision Detection 
	void ApplyImpulse(Vector3 impulse) { _velocity += impulse;}
	void SetUseWind(bool useWind) { _useWind = useWind; }

	void SetUseEuler(bool Euler) { useSIEuler = Euler; }
	void SetUseVerlet(bool Verlet) { useVerlet = Verlet; }

	bool GetUseVelet() { return useVerlet; }
	bool GetUseEuler() { return useSIEuler; }
	Vector3 _oldAccel;
protected:
	Transform* _transform;
	Collider* _collider = nullptr;
	Vector3 _velocity;
	Vector3 _acceleration;
	Vector3 _netForce;
	Vector3 _gravity = Vector3(0, -9.81f, 0);
	Vector3 _friction;
	Vector3 _drag;
	Vector3 _lift;
	Vector3 _thrust;
	Vector3 _wind;
	


	float _mass = 0.01f;
	bool _useAcceleration;
	bool _useGravity;
	bool _useLift;
	bool _useThrust;
	bool _useWind;
	bool _useFriction;
	bool _useDrag;

	bool useVerlet = false;	  // Verlet	
	bool useSIEuler = true; // Semi Implicit Euler
};

