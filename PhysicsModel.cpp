#include "PhysicsModel.h"

PhysicsModel::PhysicsModel()
{
}

PhysicsModel::PhysicsModel(Transform* transform, float mass)
{
	// Creates a new transform when constructed
	_transform = transform;
	_mass = mass;
}

void PhysicsModel::Update(float deltaTime)
{

	if (_mass == 0)
	{
		return;
	}

	Vector3 position = _transform->GetPosition();

	
	// If Gravity is on
	if (_useGravity)
	{
		// Apply Gravity Force
		_netForce += GravityForce();
		
	}

	if (_useFriction)
	{
		// Apply Friction Force
		_netForce += CalculateFrictionForce();
	}
	
	if (_useLift)
	{
		_netForce += CalculateLiftForce();
	}

	if (_useThrust)
	{
		_netForce += CalculateThrustForce(deltaTime);
	}
	if (_useWind)
	{
		_netForce += CalculateWindForce();
	}

	if (_useDrag)
	{
		_netForce += CalculateDragForce();
	}
	// If verlet is on
	//_oldAccel = _acceleration;
	// Acceleration is on, factor it into the calculation
	if (_useAcceleration)
	{
		// Apply Drag

		_acceleration += _netForce / _mass;
		if (useSIEuler)
		{
			_velocity += _acceleration * deltaTime;
		}
	}
	
	if (useSIEuler && !useVerlet)
	{
		position += _velocity * deltaTime;

		_transform->SetPosition(position);
	}
	// Adjust Position


	if (useVerlet && !useSIEuler)
	{

		Vector3 nextPos = _transform->GetPosition() + _velocity * deltaTime + (0.5f * _oldAccel) * pow(deltaTime, 2);

		Vector3 nextVelocity = (_velocity + _oldAccel + _acceleration) / 2;
		nextVelocity *= deltaTime;
	    _velocity = nextVelocity;

		// Set Position
		_transform->SetPosition(nextPos);
	}


	_oldAccel = _acceleration;
	// Set to 0 since it is calculated each frame
	_acceleration = Vector3(0, 0, 0);
	_drag = Vector3(0, 0, 0);
	//_friction = Vector3(0, 0, 0);
	_wind = Vector3(0, 0, 0);
	_lift = Vector3(0, 0, 0);
	_thrust = Vector3(0,0,0);
	_netForce = Vector3(0, 0, 0);
}

Vector3 PhysicsModel::CalculateDragForce()
{
	// Drag = coefficient * density * (velocity squared / 2) * referance area
	// Set Cube Coefficient
	float cubeCoefficient = 1.05f;
	// Set Density of Fluid
	float densityOfFluid = 0.8f;
	// Set Squared Velocity
	Vector3 velocitySqrd;
	velocitySqrd.x = _velocity.x * _velocity.x;
	velocitySqrd.y = _velocity.y * _velocity.y;
	velocitySqrd.z = _velocity.z * _velocity.z;
	// Set a Ref Area 
	float refArea = 0.5f;
	// Calculate Drag
	_drag = cubeCoefficient * densityOfFluid *  -(velocitySqrd / 2) * refArea;
	// Return drag
	return _drag;

}

Vector3 PhysicsModel::CalculateFrictionForce()
{
	// Friction = coefficient of friction * normal force
	// Set Coefficient of Friction
	float coefficientOfFriction = 1.0f;
	// Calculate Normal Force
	Vector3 normalForce = -GravityForce();
	// Return the calculation of friction
	return coefficientOfFriction * normalForce;

}

Vector3 PhysicsModel::CalculateLiftForce()
{
	// Lift = coefficient * (density * velocity squared / 2) * wing area

	float coefficient = 0.8f;

	float density = _mass;

	Vector3 velocitySquared;
	velocitySquared.x = _velocity.x * _velocity.x;
	velocitySquared.y = _velocity.y * _velocity.y;
	velocitySquared.z = _velocity.z * _velocity.z;

	float wingArea = 0.02f;

	_lift = coefficient * ((density * velocitySquared) / 2) * wingArea;


	return _lift;
}

Vector3 PhysicsModel::CalculateThrustForce(float deltaTime)
{
	// Thrust = Mass Flow Rate * velocity of the exhaust + (pressure at the exhaust - atmospheric pressure) * area of the nozzle 

	float massFlowRate = _mass / deltaTime;
	
	Vector3 velocityExhaust = /*_velocity*/ Vector3(0.001f, 0.001f, 0.001f);

	if (_velocity.Magnitude() == 0)
	{
		return Vector3(0, 0, 0);
	}

	//if (_velocity.Magnitude() > 10)
	//{
	//	_velocity /= 2;
	//}

	Vector3 pressureExhaust = Vector3(10.0f, 10.0f, 0);
	Vector3 atmosphericPressure = Vector3(5.0f, 5.0f, 0.0f);
	float areaNozzle = 1.2f;
	

	// Calcuate thrust
	_thrust = massFlowRate * velocityExhaust + (pressureExhaust - atmosphericPressure) * areaNozzle;

	return _thrust;

}

Vector3 PhysicsModel::CalculateWindForce()
{
	// Wind = drag coefficient * wind velocity * projected area (simple equation)

	float dragCoefficient = 1.05f;
	Vector3 windVelocity = _velocity;
//	windVelocity.x *= 2.0f;
	windVelocity.y *= -3.0f;
	//windVelocity.y *= -3.0f;
	float area = 1.5f;
	float p = 1.293f;		// p = density of air 

	_wind = p * dragCoefficient * windVelocity * area;
	return _wind;
}
