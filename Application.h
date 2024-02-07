#pragma once

#include <windows.h>
#include <d3d11_4.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <vector>
#include "Loaders\DDSTextureLoader.h"
#include "Loaders\OBJLoader.h"
#include "resource.h"
#include "Camera.h"
#include "Structures.h"
#include "GameObject.h"
#include "Vector3.h"
#include <iostream>;
#include "Timer.h"
#include "Debug.h"
#include "SphereCollider.h"
#include "BoxCollider.h"

using namespace DirectX;

class Application
{
private:
	HINSTANCE               _hInst;
	HWND                    _hWnd;
	D3D_DRIVER_TYPE         _driverType;
	D3D_FEATURE_LEVEL       _featureLevel;
	ID3D11Device*           _pd3dDevice;
	ID3D11DeviceContext*    _pImmediateContext;
	IDXGISwapChain*         _pSwapChain;
	ID3D11RenderTargetView* _pRenderTargetView;
	ID3D11VertexShader*     _pVertexShader;
	ID3D11PixelShader*      _pPixelShader;
	ID3D11InputLayout*      _pVertexLayout;

	ID3D11Buffer*           _pVertexBuffer;
	ID3D11Buffer*           _pIndexBuffer;

	ID3D11Buffer*           _pPlaneVertexBuffer;
	ID3D11Buffer*           _pPlaneIndexBuffer;

	ID3D11Buffer*           _pConstantBuffer;

	ID3D11DepthStencilView* _depthStencilView = nullptr;
	ID3D11Texture2D* _depthStencilBuffer = nullptr;

	ID3D11ShaderResourceView * _pTextureRV = nullptr;
	ID3D11ShaderResourceView * _pGroundTextureRV = nullptr;
	ID3D11ShaderResourceView * _pAirplaneTextureRV = nullptr;

	ID3D11SamplerState * _pSamplerLinear = nullptr;

	MeshData objMeshData;
	MeshData sphereMeshData;

	Light basicLight;

	vector<GameObject *> _gameObjects;
	vector<GameObject *> _particleGO;
	GameObject* selectedGameobject;
	Timer _timer;

	Camera * _camera = nullptr;
	float _cameraOrbitRadius = 7.0f;
	float _cameraOrbitRadiusMin = 2.0f;
	float _cameraOrbitRadiusMax = 50.0f;
	float _cameraOrbitAngleXZ = -90.0f;
	float _cameraSpeed = 0.05f;

	UINT _WindowHeight;
	UINT _WindowWidth;

	// Render dimensions - Change here to alter screen resolution
	UINT _renderHeight = 1080;
	UINT _renderWidth = 1920;

	ID3D11DepthStencilState* DSLessEqual;
	ID3D11RasterizerState* RSCullNone;

	ID3D11RasterizerState* CCWcullMode;
	ID3D11RasterizerState* CWcullMode;

private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShadersAndInputLayout();
	HRESULT InitVertexBuffer();
	HRESULT InitIndexBuffer();

	void moveForward(GameObject* gameobject);
	void moveBackward(GameObject* gameobject);
	void moveRight(GameObject* gameobject);
	void moveLeft(GameObject* gameobject);
	void moveUp(GameObject* gameobject);
	void moveDown(GameObject* gameobject);

	BoxCollider* boxCollider;
	BoxCollider* boxCollider2;
	BoxCollider* planeBoxCollider;
	BoxCollider* boxCollider3;
	ParticleModel* pm;
	ParticleModel* pm2;
	ParticleModel* pm3;
	ParticleModel* pm4;
	ParticleModel* pm5;
	SphereCollider* _sphereCollider;
	SphereCollider* _sphereCollider2;
public:
	Application();
	~Application();
	bool _pickedUP = false;
	float _timeAlive = 0;
	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	bool HandleKeyboard(MSG msg);

	void Update();
	void ResolveCollisions();
	void Draw();
};

