#pragma once
#include <directxmath.h>
#include <d3d11_4.h>
#include <string>
#include "Structures.h"
//#include "Vector3.h"
#include "Quaternion.h"
class Transform
{
public:

	Transform();

	~Transform();


	// Setters and Getters for position/rotation/scale
	void SetPosition(Vector3 position) { _position = position; }
	void SetPosition(float x, float y, float z) { _position.x = x; _position.y = y; _position.z = z; }
	Vector3 GetPosition() { return _position; }

	void SetScale(Vector3 scale) { _scale = scale; }
	void SetScale(float x, float y, float z) { _scale.x = x; _scale.y = y; _scale.z = z; }
	Vector3 GetScale() const { return _scale; }

	void SetRotation(Vector3 rotation) { _orientation = MakeQFromEulerAngles(rotation.x, rotation.y, rotation.z); }
	void SetRotation(float x, float y, float z) { _orientation = MakeQFromEulerAngles(x, y, z); }
	Vector3 GetRotation() const { return MakeEulerAnglesFromQ(_orientation); }

	void SetOrientation(Quaternion orientation) { _orientation = orientation; }
	Quaternion GetOrientation() { return _orientation; }
	
	// Getters and Setters for OBJECT WORLD MATRIX
	XMMATRIX GetWorldMatrix();
	void SetWorldMatrix(XMMATRIX world);

private:
	Vector3 _position;
	Quaternion _orientation;
	Vector3 _scale;

	XMFLOAT4X4 _world;



};

