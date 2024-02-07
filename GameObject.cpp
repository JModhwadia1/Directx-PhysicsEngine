#include "GameObject.h"

GameObject::GameObject(string type, Appearance* appearance) : _type(type), _appearance(appearance)
{
	_parent = nullptr;
	_transform = new Transform();
	//_particleModel = new ParticleModel(_transform, 1.0f);
	//_rigidbodyModel = new RigidbodyModel(_transform, 1.0f);
}

GameObject::GameObject(string type, Appearance* appearance, ParticleModel* particleModel) : _type(type), _appearance(appearance), _particleModel(particleModel)
{
	_parent = nullptr;
	_transform = new Transform();

}

GameObject::~GameObject()
{
	/*_textureRV = nullptr;*/
	_parent = nullptr;
}

void GameObject::Update(float t)
{
	// Update Physics Model
	
	
	_physicsModel->Update(t);
	// Update Transform
	_transform->GetWorldMatrix();

	if (_parent != nullptr)
	{
		//If there is a parent, factor it into the calculation
		_transform->SetWorldMatrix(_transform->GetWorldMatrix() * _parent->GetTransform()->GetWorldMatrix());

		//XMStoreFloat4x4(_transform.set, this->GetWorldMatrix() * _parent->GetWorldMatrix());
	}
	else 
	{
		_transform->SetWorldMatrix(_transform->GetWorldMatrix());
	}
	
}

void GameObject::Draw(ID3D11DeviceContext * pImmediateContext)
{
	
	_appearance->Draw(pImmediateContext);

}
