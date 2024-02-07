#pragma once

#include <directxmath.h>
#include <d3d11_4.h>
#include <string>
#include "Structures.h"
#include "Vector3.h"
#include "Transform.h"
#include "Appearance.h"
#include "PhysicsModel.h"
#include "ParticleModel.h"
#include "RigidbodyModel.h"

using namespace DirectX;
using namespace std;

class GameObject
{
public:
	GameObject(string type, Appearance* appearance);
	GameObject(string type, Appearance* appearance, ParticleModel* particleModel);
	~GameObject();



	string GetType() const { return _type; }

	Transform* GetTransform() { return _transform; }
	Appearance* GetAppearance() { return _appearance; }
	PhysicsModel* GetPhysicsModel() { return _physicsModel; }
	void SetPhysicsModel(PhysicsModel* physicsModel) { _physicsModel = physicsModel; }

	void SetParent(GameObject * parent) { _parent = parent; }
	
	void Update(float t);
	void Draw(ID3D11DeviceContext * pImmediateContext);

	//PhysicsModel* GetPhysicsModel() { return _rigidbodyModel; }
private:
	
	Transform* _transform;
	string _type;


	GameObject * _parent;

	Appearance* _appearance;
	ParticleModel* _particleModel;
	RigidbodyModel* _rigidbodyModel;
	PhysicsModel* _physicsModel;
};

