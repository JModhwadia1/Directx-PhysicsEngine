#include "Transform.h"

Transform::Transform()
{
	// Init position
	_position = Vector3();
	// Init quarternion
	_orientation = Quaternion();
	// Init scale
	_scale = Vector3(1.0f, 1.0f, 1.0f);
}

XMMATRIX Transform::GetWorldMatrix()
{
	// Calculate world matrix
	// Get the scale matrix
	XMMATRIX scale = XMMatrixScaling(_scale.x, _scale.y, _scale.z);
	// Get rotation matrix using a quaternion
	XMFLOAT4 vf4(_orientation.GetVector().x, _orientation.GetVector().y, _orientation.GetVector().z, _orientation.GetScalar());
	XMVECTOR quaternionXMV = XMLoadFloat4(&vf4);
	XMMATRIX rotation = XMMatrixRotationQuaternion(quaternionXMV);
	// Get the translation matrix
	XMMATRIX translation = XMMatrixTranslation(_position.x, _position.y, _position.z);
	// Store the world matrix
	XMStoreFloat4x4(&_world, scale * rotation * translation);


	// Return the world matrix
	return XMLoadFloat4x4(&_world);
}

void Transform::SetWorldMatrix(XMMATRIX world)
{
	// Set the world matrix using the param passed in
	XMStoreFloat4x4(&_world, world);
}


Transform::~Transform()
{

}

