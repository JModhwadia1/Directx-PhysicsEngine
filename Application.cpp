#include "Application.h"
using namespace std;
#define NUMBEROFCUBES 3
#define FPS60 0.016f

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

bool Application::HandleKeyboard(MSG msg)
{
	XMFLOAT3 cameraPosition = _camera->GetPosition();

	float cameraSpeedMovement = 5.0f;
	switch (msg.wParam)
	{
	case VK_UP:
		_cameraOrbitRadius = max(_cameraOrbitRadiusMin, _cameraOrbitRadius - (cameraSpeedMovement * 0.2f));
		return true;
		break;

	case VK_DOWN:
		_cameraOrbitRadius = min(_cameraOrbitRadiusMax, _cameraOrbitRadius + (cameraSpeedMovement* 0.2f));
		return true;
		break;

	case VK_RIGHT:
		_cameraOrbitAngleXZ -= _cameraSpeed;
		return true;
		break;

	case VK_LEFT:
		_cameraOrbitAngleXZ += _cameraSpeed;
		return true;
		break;
	}

	return false;
}

Application::Application()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	_pVertexLayout = nullptr;
	_pVertexBuffer = nullptr;
	_pIndexBuffer = nullptr;
	_pConstantBuffer = nullptr;
	CCWcullMode=nullptr;
	CWcullMode= nullptr;
	DSLessEqual = nullptr;
	RSCullNone = nullptr;
	 _WindowHeight = 0;
	 _WindowWidth = 0;

	 // Create Timer
	 _timer = Timer();
}

Application::~Application()
{
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{

	

	HRESULT hr = 0;

    if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
        return E_FAIL;
	}

    RECT rc;
	if (!GetClientRect(_hWnd, &rc)) { return E_FAIL; }
    _WindowWidth = rc.right - rc.left;
    _WindowHeight = rc.bottom - rc.top;

    if (FAILED(InitDevice()))
    {
        Cleanup();

        return E_FAIL;
    }

	
	hr = CreateDDSTextureFromFile(_pd3dDevice, L"Assets\\Textures\\stone.dds", nullptr, &_pTextureRV);
	hr = CreateDDSTextureFromFile(_pd3dDevice, L"Assets\\Textures\\floor.dds", nullptr, &_pGroundTextureRV);
	hr = CreateDDSTextureFromFile(_pd3dDevice, L"Assets\\Textures\\Hercules_COLOR.dds", nullptr, &_pAirplaneTextureRV);
	
	if (FAILED(hr))
	{
		return E_FAIL;
	}

    // Setup Camera
	XMFLOAT3 eye = XMFLOAT3(0.0f, 2.0f, -1.0f);
	XMFLOAT3 at = XMFLOAT3(0.0f, 2.0f, 0.0f);
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);

	_camera = new Camera(eye, at, up, (float)_renderWidth, (float)_renderHeight, 0.01f, 200.0f);

	// Setup the scene's light
	basicLight.AmbientLight = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	basicLight.DiffuseLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	basicLight.SpecularLight = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	basicLight.SpecularPower = 20.0f;
	basicLight.LightVecW = XMFLOAT3(0.0f, 1.0f, -1.0f);

	std::shared_ptr<Geometry> SPairplaneGeometry = make_shared<Geometry>();
	objMeshData = OBJLoader::Load("Assets/3DModels/Hercules.obj", _pd3dDevice);
	SPairplaneGeometry->indexBuffer = objMeshData.IndexBuffer;
	SPairplaneGeometry->numberOfIndices = objMeshData.IndexCount;
	SPairplaneGeometry->vertexBuffer = objMeshData.VertexBuffer;
	SPairplaneGeometry->vertexBufferOffset = objMeshData.VBOffset;
	SPairplaneGeometry->vertexBufferStride = objMeshData.VBStride;

	std::shared_ptr<Geometry> SPsphereGeometry = make_shared<Geometry>();
	sphereMeshData = OBJLoader::Load("Assets/3DModels/sphere.obj", _pd3dDevice);
	SPsphereGeometry->indexBuffer = sphereMeshData.IndexBuffer;
	SPsphereGeometry->numberOfIndices = sphereMeshData.IndexCount;
	SPsphereGeometry->vertexBuffer = sphereMeshData.VertexBuffer;
	SPsphereGeometry->vertexBufferOffset = sphereMeshData.VBOffset;
	SPsphereGeometry->vertexBufferStride = sphereMeshData.VBStride;


	
	std::shared_ptr<Geometry> SPcubeGeometry = make_shared<Geometry>();
	SPcubeGeometry->indexBuffer = _pIndexBuffer;
	SPcubeGeometry->vertexBuffer = _pVertexBuffer;
	SPcubeGeometry->numberOfIndices = 36;
	SPcubeGeometry->vertexBufferOffset = 0;
	SPcubeGeometry->vertexBufferStride = sizeof(SimpleVertex);

	std::shared_ptr<Geometry> SPplaneGeometry = make_shared<Geometry>();
	SPplaneGeometry->indexBuffer = _pPlaneIndexBuffer;
	SPplaneGeometry->vertexBuffer = _pPlaneVertexBuffer;
	SPplaneGeometry->numberOfIndices = 6;
	SPplaneGeometry->vertexBufferOffset = 0;
	SPplaneGeometry->vertexBufferStride = sizeof(SimpleVertex);

	std::shared_ptr<Material> SPshinyMaterial = make_shared<Material>();
	SPshinyMaterial->ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	SPshinyMaterial->diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SPshinyMaterial->specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	SPshinyMaterial->specularPower = 10.0f;

	std::shared_ptr<Material> SPnoSpecMaterial = make_shared<Material>();
	SPnoSpecMaterial->ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	SPnoSpecMaterial->diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SPnoSpecMaterial->specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	SPnoSpecMaterial->specularPower = 0.0f;

	
	Appearance* floorAppearance= new Appearance(SPnoSpecMaterial, SPplaneGeometry);
	

	//Appearance* floorAppearance3 = new Appearance(noSpecMaterial, planeGeometry);
	
	RigidbodyModel* floorModel = new RigidbodyModel(new Transform(), 10.0f);
	floorModel->SetUseGravity(false);
	/*floorModel->SetUseLift(false);
	floorModel->SetUseThrust(false);*/

	GameObject* gameObject = new GameObject("Floor",floorAppearance);
	gameObject->SetPhysicsModel(floorModel);
	gameObject->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
	gameObject->GetTransform()->SetScale(15.0f, 15.0f, 15.0f);
	gameObject->GetTransform()->SetRotation(90.0f, 0.0f, 0.0f);
	gameObject->GetAppearance()->SetTextureRV(_pGroundTextureRV);
	_gameObjects.push_back(gameObject);
	
	Appearance* cubeAppearance = new Appearance(SPnoSpecMaterial, SPcubeGeometry);

	for (auto i = 0; i < NUMBEROFCUBES; i++)
	{
		
		gameObject = new GameObject("Cube " + to_string(i), cubeAppearance);
		
		gameObject->GetTransform()->SetScale(1.0f, 1.0f, 1.0f);
		gameObject->GetTransform()->SetPosition(-3.0f + (i * 5.0f), 1.0f, 10.0f);
		gameObject->GetAppearance()->SetTextureRV(_pTextureRV);


		_gameObjects.push_back(gameObject);
	}
	RigidbodyModel* rbModel = new RigidbodyModel(_gameObjects[1]->GetTransform(), 10.0f);
	rbModel->SetUseGravity(true);
	rbModel->SetUseFriction(true);
	rbModel->SetUseDrag(true);
	rbModel->SetUseAcceleration(true);
	rbModel->SetUseLift(false);
	rbModel->SetUseThrust(false);
	rbModel->SetUseWind(false);
	rbModel->SetUseVerlet(true);
	rbModel->SetUseEuler(false);

     boxCollider = new BoxCollider(_gameObjects[1]->GetTransform(), Vector3(1, 1, 1));
	_gameObjects[1]->SetPhysicsModel(rbModel);
	_gameObjects[1]->GetPhysicsModel()->SetMass(10.0f);
	_gameObjects[1]->GetPhysicsModel()->SetCollider(boxCollider);
	
	//_sphereCollider = new SphereCollider(_gameObjects[1]->GetTransform(), 1.0f);
	////_gameObjects[1]->GetPhysicsModel()->SetVelocity(Vector3(0, 1, 0));	// Set Constant Velocity value
	//_sphereCollider2 = new SphereCollider(_gameObjects[2]->GetTransform(), 1.0f);
	rbModel = new RigidbodyModel(_gameObjects[2]->GetTransform(), 1.0f);
	rbModel->SetUseGravity(true);
	rbModel->SetUseFriction(true);
	rbModel->SetUseDrag(true);
	rbModel->SetUseAcceleration(true);
	rbModel->SetUseLift(false);
	rbModel->SetUseThrust(false);
	rbModel->SetUseWind(false);
	rbModel->SetUseVerlet(true);
	rbModel->SetUseEuler(false);

	boxCollider2 = new BoxCollider(_gameObjects[2]->GetTransform(), Vector3(1, 1, 1));
	_gameObjects[2]->SetPhysicsModel(rbModel);
	_gameObjects[2]->GetPhysicsModel()->SetCollider(boxCollider2);


	// Box 3
	rbModel = new RigidbodyModel(_gameObjects[3]->GetTransform(), 10.0f);
	rbModel->SetUseGravity(true);
	rbModel->SetUseFriction(true);
	rbModel->SetUseDrag(true);
	rbModel->SetUseAcceleration(true);
	rbModel->SetUseLift(false);
	rbModel->SetUseThrust(false);
	boxCollider3 = new BoxCollider(_gameObjects[3]->GetTransform(), Vector3(1, 1, 1));
	_gameObjects[3]->SetPhysicsModel(rbModel);
	_gameObjects[3]->GetPhysicsModel()->SetCollider(boxCollider3);
	

	Appearance* airplaneAppearance = new Appearance(SPshinyMaterial, SPairplaneGeometry);
	gameObject = new GameObject("Airplane", airplaneAppearance);
	gameObject->GetTransform()->SetScale(0.5f, 0.5f, 0.5f);
	gameObject->GetTransform()->SetPosition(-6.0f, 0.5f, 10.0f);
	gameObject->GetTransform()->SetRotation(0, -90, 0);
	gameObject->GetAppearance()->SetTextureRV(_pAirplaneTextureRV);
	_gameObjects.push_back(gameObject);

	rbModel = new RigidbodyModel(_gameObjects[4]->GetTransform(), 70.0f);
	rbModel->SetUseGravity(false);
	rbModel->SetUseFriction(false);
	rbModel->SetUseLift(false);
	rbModel->SetUseThrust(false);
	rbModel->SetUseWind(true);
	gameObject->SetPhysicsModel(rbModel);


	Appearance* sphereAppearance = new Appearance(SPshinyMaterial, SPsphereGeometry);
	gameObject = new GameObject("Sphere", sphereAppearance);
	gameObject->GetTransform()->SetPosition(13.0f, 1.0f, 10.0f);
	gameObject->GetAppearance()->SetTextureRV(_pAirplaneTextureRV);
	_gameObjects.push_back(gameObject);

	rbModel = new RigidbodyModel(_gameObjects[5]->GetTransform(), 10.0f);
	rbModel->SetUseGravity(true);
	rbModel->SetUseDrag(true);
	rbModel->SetUseAcceleration(true);
	rbModel->SetUseFriction(true);
	rbModel->SetUseLift(false);
	rbModel->SetUseThrust(false);
	rbModel->SetUseWind(false);
	
	_sphereCollider = new SphereCollider(_gameObjects[5]->GetTransform(), 1.0f);
	_gameObjects[5]->SetPhysicsModel(rbModel);
	//[5]->GetPhysicsModel()->SetMass(10);
	_gameObjects[5]->GetPhysicsModel()->SetCollider(_sphereCollider);

	gameObject = new GameObject("Sphere2", sphereAppearance);
	gameObject->GetTransform()->SetPosition(17.0f, 1.0f, 10.0f);
	gameObject->GetAppearance()->SetTextureRV(_pAirplaneTextureRV);

	_gameObjects.push_back(gameObject);
	rbModel = new RigidbodyModel(_gameObjects[6]->GetTransform(), 1.0f);
	rbModel->SetUseGravity(true);
	rbModel->SetUseDrag(true);
	rbModel->SetUseAcceleration(true);
	rbModel->SetUseFriction(true);
	rbModel->SetUseLift(false);
	rbModel->SetUseThrust(false);
	rbModel->SetUseWind(false);
	_sphereCollider2 = new SphereCollider(_gameObjects[6]->GetTransform(), 1.0f);
	_gameObjects[6]->SetPhysicsModel(rbModel);
	_gameObjects[6]->GetPhysicsModel()->SetCollider(_sphereCollider2);
	// Need to be in a for loop

	for (int i = 0; i < 5; i++)
	{

		gameObject = new GameObject("particleGO", sphereAppearance);
		gameObject->GetTransform()->SetPosition(-3.0f + (i * 2.0f), 10.0f, 10.0f);
		gameObject->GetTransform()->SetScale(.3f, .3f, .3f);
		gameObject->GetAppearance()->SetTextureRV(_pAirplaneTextureRV);
		_particleGO.push_back(gameObject);
	}
	// 1st particle
	pm = new ParticleModel(_particleGO[0]->GetTransform(), 2.0f, Vector3(0, 0, 0), false);
	pm->SetUseGravity(false);
	pm->SetUseFriction(false);
	pm->SetUseDrag(false);
	pm->SetVelocity(Vector3(-2.0f, 10.0f, 0.0f));
	pm->SetUseLift(false);
	pm->SetUseWind(false);
	pm->SetUseThrust(false);
	pm->SetMass(1.0f);
	_particleGO[0]->SetPhysicsModel(pm);
	// 2nd particle
	pm2 = new ParticleModel(_particleGO[1]->GetTransform(), 2.0f, Vector3(0, 0, 0), false);
	pm2->SetUseGravity(false);
	pm2->SetUseFriction(false);
	pm2->SetUseDrag(false);
	pm2->SetVelocity(Vector3(-2.0f, 10.0f, 0.0f));
	pm2->SetUseLift(false);
	pm2->SetUseWind(false);
	pm2->SetUseThrust(false);
	pm2->SetMass(1.0f);
	_particleGO[1]->SetPhysicsModel(pm2);
	// 3rd particle
	pm3 = new ParticleModel(_particleGO[2]->GetTransform(), 2.0f, Vector3(0, 0, 0), false);
	pm3->SetUseGravity(false);
	pm3->SetUseFriction(false);
	pm3->SetUseDrag(false);
	pm3->SetVelocity(Vector3(0, 10, 0));
	pm3->SetUseLift(false);
	pm3->SetUseWind(false);
	pm3->SetUseThrust(false);
	pm3->SetMass(1.0f);
	_particleGO[2]->SetPhysicsModel(pm3);
	// 4th particle
	pm4 = new ParticleModel(_particleGO[3]->GetTransform(), 2.0f, Vector3(0, 0, 0), false);
	pm4->SetUseGravity(false);
	pm4->SetUseFriction(false);
	pm4->SetUseDrag(false);
	pm4->SetVelocity(Vector3(2, 10, 0));
	pm4->SetUseLift(false);
	pm4->SetUseWind(false);
	pm4->SetUseThrust(false);
	pm4->SetMass(1.0f);
	_particleGO[3]->SetPhysicsModel(pm4);
	// 5th particle
	pm5 = new ParticleModel(_particleGO[4]->GetTransform(), 2.0f, Vector3(0, 0, 0), false);
	pm5->SetUseGravity(false);
	pm5->SetUseFriction(false);
	pm5->SetUseDrag(false);
	pm5->SetVelocity(Vector3(2, 10, 0));
	pm5->SetUseLift(false);
	pm5->SetUseWind(false);
	pm5->SetUseThrust(false);
	pm5->SetMass(1.0f);
	_particleGO[4]->SetPhysicsModel(pm5);
	
	
	
	

	selectedGameobject = _gameObjects[1];

	int shmeck = SPcubeGeometry.use_count();

	return S_OK;
}

HRESULT Application::InitShadersAndInputLayout()
{
	HRESULT hr;

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.hlsl", "VS", "vs_4_0", &pVSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The HLSL file cannot be compiled.  Error output to Visual Studio Console.", L"Error", MB_OK);
        return hr;
    }

	// Create the vertex shader
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);

	if (FAILED(hr))
	{	
		pVSBlob->Release();
        return hr;
	}

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.hlsl", "PS", "ps_4_0", &pPSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The HLSL file cannot be compiled.  Error output to Visual Studio Console.", L"Error", MB_OK);
        return hr;
    }

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);
	pPSBlob->Release();

    if (FAILED(hr))
        return hr;
	
    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
                                        pVSBlob->GetBufferSize(), &_pVertexLayout);
	pVSBlob->Release();

	if (FAILED(hr))
        return hr;

    // Set the input layout
    _pImmediateContext->IASetInputLayout(_pVertexLayout);

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = _pd3dDevice->CreateSamplerState(&sampDesc, &_pSamplerLinear);

	return hr;
}

HRESULT Application::InitVertexBuffer()
{
	HRESULT hr;

    // Create vertex buffer
    SimpleVertex vertices[] =
    {
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },

		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
    };

    D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * 24;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = vertices;

    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pVertexBuffer);

    if (FAILED(hr))
        return hr;

	// Create vertex buffer
	SimpleVertex planeVertices[] =
	{
		{ XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 5.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(5.0f, 5.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(5.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
	};

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = planeVertices;

	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pPlaneVertexBuffer);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::InitIndexBuffer()
{
	HRESULT hr;

    // Create index buffer
    WORD indices[] =
    {
		3, 1, 0,
		2, 1, 3,

		6, 4, 5,
		7, 4, 6,

		11, 9, 8,
		10, 9, 11,

		14, 12, 13,
		15, 12, 14,

		19, 17, 16,
		18, 17, 19,

		22, 20, 21,
		23, 20, 22
    };

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 36;     
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = indices;
    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pIndexBuffer);

    if (FAILED(hr))
        return hr;

	// Create plane index buffer
	WORD planeIndices[] =
	{
		0, 3, 1,
		3, 2, 1,
	};

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 6;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = planeIndices;
	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pPlaneIndexBuffer);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW );
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    _hInst = hInstance;
    RECT rc = {0, 0, 960, 540};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    _hWnd = CreateWindow(L"TutorialWindowClass", L"FGGC Semester 2 Framework", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                         nullptr);
    if (!_hWnd)
		return E_FAIL;

    ShowWindow(_hWnd, nCmdShow);

    return S_OK;
}

HRESULT Application::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob != nullptr)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

        if (pErrorBlob) pErrorBlob->Release();

        return hr;
    }

    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

HRESULT Application::InitDevice()
{
    HRESULT hr = S_OK;

    UINT createDeviceFlags = 0;

#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };

    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	UINT sampleCount = 4;

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = _renderWidth;
    sd.BufferDesc.Height = _renderHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = _hWnd;
	sd.SampleDesc.Count = sampleCount;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        _driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                           D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_featureLevel, &_pImmediateContext);
        if (SUCCEEDED(hr))
            break;
    }

    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(hr))
        return hr;

    hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
    pBackBuffer->Release();

    if (FAILED(hr))
        return hr;

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)_renderWidth;
    vp.Height = (FLOAT)_renderHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    _pImmediateContext->RSSetViewports(1, &vp);

	InitShadersAndInputLayout();

	InitVertexBuffer();
	InitIndexBuffer();

    // Set primitive topology
    _pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
    hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);

    if (FAILED(hr))
        return hr;

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = _renderWidth;
	depthStencilDesc.Height = _renderHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = sampleCount;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	_pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);
	_pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);

	_pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);

	// Rasterizer
	D3D11_RASTERIZER_DESC cmdesc;

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_NONE;
	hr = _pd3dDevice->CreateRasterizerState(&cmdesc, &RSCullNone);

	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	_pd3dDevice->CreateDepthStencilState(&dssDesc, &DSLessEqual);

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));

	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;

	cmdesc.FrontCounterClockwise = true;
	hr = _pd3dDevice->CreateRasterizerState(&cmdesc, &CCWcullMode);

	cmdesc.FrontCounterClockwise = false;
	hr = _pd3dDevice->CreateRasterizerState(&cmdesc, &CWcullMode);

	

    return S_OK;
}

void Application::Cleanup()
{
    if (_pImmediateContext) _pImmediateContext->ClearState();
	if (_pSamplerLinear) _pSamplerLinear->Release();

	if (_pTextureRV) _pTextureRV->Release();

	if (_pGroundTextureRV) _pGroundTextureRV->Release();

	if (_pAirplaneTextureRV) _pAirplaneTextureRV->Release();

    if (_pConstantBuffer) _pConstantBuffer->Release();

    if (_pVertexBuffer) _pVertexBuffer->Release();
    if (_pIndexBuffer) _pIndexBuffer->Release();
	if (_pPlaneVertexBuffer) _pPlaneVertexBuffer->Release();
	if (_pPlaneIndexBuffer) _pPlaneIndexBuffer->Release();

    if (_pVertexLayout) _pVertexLayout->Release();
    if (_pVertexShader) _pVertexShader->Release();
    if (_pPixelShader) _pPixelShader->Release();
    if (_pRenderTargetView) _pRenderTargetView->Release();
    if (_pSwapChain) _pSwapChain->Release();
    if (_pImmediateContext) _pImmediateContext->Release();
    if (_pd3dDevice) _pd3dDevice->Release();
	if (_depthStencilView) _depthStencilView->Release();
	if (_depthStencilBuffer) _depthStencilBuffer->Release();

	if (DSLessEqual) DSLessEqual->Release();
	if (RSCullNone) RSCullNone->Release();

	if (CCWcullMode) CCWcullMode->Release();
	if (CWcullMode) CWcullMode->Release();

	if (_camera)
	{
		delete _camera;
	}

	for (auto gameObject : _gameObjects)
	{
		if (gameObject)
		{
			delete gameObject;
		}
	}
}

void Application::moveForward(GameObject* gameobject)
{
	gameobject->GetPhysicsModel()->AddForce(Vector3(0, 0, -1.0f));	
}

void Application::moveBackward(GameObject* gameobject)
{
	gameobject->GetPhysicsModel()->AddForce(Vector3(0, 0, 1.0f));
}

void Application::moveLeft(GameObject* gameobject)
{
	if (gameobject == _gameObjects[1] || gameobject == _gameObjects[2])
	{
		if (gameobject->GetPhysicsModel()->GetUseEuler() == true)
		{
			gameobject->GetPhysicsModel()->AddForce(Vector3(-1.0f, 0, 0));
		}
		else if (gameobject->GetPhysicsModel()->GetUseVelet() == true)
		{
			gameobject->GetPhysicsModel()->AddForce(Vector3(-100.0f, 0, 0));
		}


	}
	else
	{
		gameobject->GetPhysicsModel()->AddForce(Vector3(-1.0f, 0, 0));

	}


	
}
void Application::moveRight(GameObject* gameobject)
{
	if (gameobject == _gameObjects[1] || gameobject == _gameObjects[2])
	{
		if (gameobject->GetPhysicsModel()->GetUseEuler() == true)
		{
			gameobject->GetPhysicsModel()->AddForce(Vector3(1.0f, 0, 0));
		}
		else if (gameobject->GetPhysicsModel()->GetUseVelet() == true)
		{
			gameobject->GetPhysicsModel()->AddForce(Vector3(100.0f, 0, 0));
		}
		

	}
	else
	{
		gameobject->GetPhysicsModel()->AddForce(Vector3(1.0f, 0, 0));

	}
}
void Application::moveUp(GameObject* gameobject)
{
	gameobject->GetPhysicsModel()->AddForce(Vector3(0.0f, 1.0f, 0));
}
void Application::moveDown(GameObject* gameobject)
{
	gameobject->GetPhysicsModel()->AddForce(Vector3(0.0f, -1.0f, 0));
}

void Application::Update()
{
    // Update our time
  

	static float accumulator = 0.0f;
	accumulator += _timer.GetDeltaTime();

#ifdef  _DEBUG
	if (accumulator > 1.0f)
	{
		accumulator = FPS60;
	}
#endif //  _DEBUG


	while ( accumulator >= FPS60)
	{
		// Call update on each particle model - make sure particle model is set on the Gamobeject
		for (int i = 0; i < _particleGO.size(); i++)
		{
			_particleGO[i]->Update(_timer.GetDeltaTime());
		}
		//_particleGO[0]->Update(_timer.GetDeltaTime());
		
		Debug::DebugPrintF("PGO 1 Velocity%f \n", _particleGO[0]->GetPhysicsModel()->GetVelocity().y);
		//Debug::DebugPrintF("real: %f %f\n", ((RigidbodyModel*)_gameObjects[1]->GetPhysicsModel())->_angularVelocity.y, ((RigidbodyModel*)_gameObjects[1]->GetPhysicsModel())->_torque.y );
			
		//pm->GetVelocity() *= 0.8f;
		if (_gameObjects[4]->GetPhysicsModel()->GetVelocity().x > 0.65f)
		{
			_gameObjects[4]->GetPhysicsModel()->SetUseLift(true);
			_gameObjects[4]->GetPhysicsModel()->SetUseThrust(true);
			_gameObjects[4]->GetTransform()->SetRotation(0, -90, 30);

		}
		if (GetAsyncKeyState('0'))
		{
			selectedGameobject->GetTransform()->SetPosition(0, 1, 10);
			if (selectedGameobject->GetPhysicsModel()->GetVelocity().Magnitude() > 0)
			{
				selectedGameobject->GetPhysicsModel()->SetVelocity(Vector3(0, 0, 0));
			}
		}

		if (GetAsyncKeyState('1'))
		{
			selectedGameobject = _gameObjects[1];
		}
		if (GetAsyncKeyState('2'))
		{
			selectedGameobject = _gameObjects[2];
		}
		if (GetAsyncKeyState('3'))
		{
			selectedGameobject = _gameObjects[4];
		}

		if (GetAsyncKeyState('4'))
		{
			selectedGameobject = _gameObjects[5];
		}
		if (GetAsyncKeyState('5'))
		{
			selectedGameobject = _gameObjects[6];
		}


		if (GetAsyncKeyState('W'))
		{
			moveForward(selectedGameobject);
		}
		if (GetAsyncKeyState('S'))
		{
			moveBackward(selectedGameobject);
		}
		if (GetAsyncKeyState('A'))
		{
			moveLeft(selectedGameobject);
		}
		if (GetAsyncKeyState('D'))
		{
			moveRight(selectedGameobject);
		}

		if (GetAsyncKeyState('E'))
		{
			moveUp(selectedGameobject);
		}
		if (GetAsyncKeyState('Q'))
		{
			moveDown(selectedGameobject);
		}
		if (GetAsyncKeyState('F'))
		{
			_gameObjects[1]->GetPhysicsModel()->AddRelativeForce(Vector3(0, 0, -1), Vector3(1, 0, -1));
			
		}


		if (GetAsyncKeyState('8'))
		{
			_gameObjects[1]->GetPhysicsModel()->SetUseEuler(true);
			_gameObjects[1]->GetPhysicsModel()->SetUseVerlet(false);

			_gameObjects[2]->GetPhysicsModel()->SetUseEuler(true);
			_gameObjects[2]->GetPhysicsModel()->SetUseVerlet(false);
;
		}
		//Debug::DebugPrintF(" Velocity %f\n", ((RigidbodyModel*)_gameObjects[1]->GetPhysicsModel())->GetVelocity().x);
	

		
		// If any of the gameobjects are below -0.5 in the y axis then enable gravity
		for (int  i = 0; i < _gameObjects.size(); i++)
		{
			//// use gravity if the y position is <= 0.5f
			//if (_gameObjects[i]->GetTransform()->GetPosition().y <= -0.5f)
			//{
			//
			//		_gameObjects[i]->GetPhysicsModel()->SetUseGravity(true);
			//	
			//}
			//// else dont use gravity - but friction is still moving it up due to having Gravity as part of the Friction calculation
			//else
			//{
			//	
			//	

			//		_gameObjects[i]->GetPhysicsModel()->SetUseGravity(false);
			//	
			//}
		}
	//	Debug::DebugPrintF("Accelration, %f", _gameObjects[1]->GetPhysicsModel()->GetVelocity().x);
		ResolveCollisions();

		if (_pickedUP == true)
		{
			_timeAlive += _timer.GetDeltaTime();
			Debug::DebugPrintF("timealive, %f", _timeAlive);
		}

		if (_timeAlive >= 5)
		{
			_pickedUP = false;
			_gameObjects[1]->GetPhysicsModel()->SetMass(10);
			_gameObjects[1]->GetTransform()->SetScale(1, 1, 1);
			_timeAlive = 0;

		}

		//// Check if both have colliders
		//if (_gameObjects[1]->GetPhysicsModel()->IsCollideable() && _gameObjects[2]->GetPhysicsModel()->IsCollideable())
		//{
		//	if (_gameObjects[1]->GetPhysicsModel()->GetCollider()->CollidesWith(*_gameObjects[2]->GetPhysicsModel()->GetCollider()))
		//	{

		//		
		//		// Get collision normal
		//		Vector3 collisionNormal = _gameObjects[1]->GetTransform()->GetPosition() - _gameObjects[2]->GetTransform()->GetPosition();
		//		// Normalize it
		//		collisionNormal.Normalize();
		//		// Restitution  - Between 0 and 1 for testing
		//		float restitution = 1.0f;
		//		// Get relative velocity // GO1 Velocity - GO2 Velocity = relative velocity
		//		Vector3 relativeVelocity = _gameObjects[1]->GetPhysicsModel()->GetVelocity() - _gameObjects[2]->GetPhysicsModel()->GetVelocity();
		//	//	Vector3 relativePosition = ((BoxCollider*)_gameObjects[1]->GetPhysicsModel()->GetCollider())->GetCenter() - ((BoxCollider*)_gameObjects[2]->GetPhysicsModel()->GetCollider())->GetCenter();
		//		// Calculate total velocity 
		//		float vj = -(1 + restitution) * collisionNormal  * relativeVelocity;
		//		// Calculate implulse
		//		float j = vj * (1 / _gameObjects[1]->GetPhysicsModel()->GetMass() + 1 / _gameObjects[2]->GetPhysicsModel()->GetMass());



		//		//// Sphere Interpenertrations
		//		//float depth = (_gameObjects[1]->GetTransform()->GetPosition() - _gameObjects[2]->GetTransform()->GetPosition()).Magnitude() - _sphereCollider->GetRadius() - _sphereCollider2->GetRadius();
		//		//_gameObjects[1]->GetTransform()->GetPosition() - collisionNormal * depth * (1 / _gameObjects[1]->GetPhysicsModel()->GetMass());
		//		//_gameObjects[2]->GetTransform()->GetPosition() + collisionNormal * depth * (1 / _gameObjects[2]->GetPhysicsModel()->GetMass());

		//		////// Box Interpenertrations
		//		//Vector3 closestPointCube1 = boxCollider->GetCenter();
		//		//Vector3 closestPointCube2=  boxCollider2->GetCenter();
		//		//
		//		//// Is GO1 center.x > GO2 Max.x?
		//		//if (closestPointCube1.x > boxCollider2->GetMax().x)
		//		//{
		//		//	// Clamp center.x to max.x
		//		//	closestPointCube1.x = boxCollider2->GetMax().x;
		//		//}
		//		//else if(closestPointCube1.x < boxCollider2->GetMin().x)
		//		//{
		//		//	closestPointCube1.x = boxCollider2->GetMin().x;
		//		//}
		//		//else
		//		//{
		//		//	closestPointCube1.x = boxCollider->GetCenter().x;
		//		//}
		//		
		//		Vector3 ApproxPointOfCollision = _gameObjects[1]->GetTransform()->GetPosition() + _gameObjects[2]->GetTransform()->GetPosition() / 2;
		//		Vector3 relativePoint1 = _gameObjects[1]->GetTransform()->GetPosition() - ApproxPointOfCollision;
		//		Vector3 relativePoint2 = _gameObjects[2]->GetTransform()->GetPosition() - ApproxPointOfCollision;


		//	
		//		// Add impulse
		//		_gameObjects[1]->GetPhysicsModel()->ApplyImpulse(1 / _gameObjects[1]->GetPhysicsModel()->GetMass() * j * collisionNormal);
		//	
		//		_gameObjects[2]->GetPhysicsModel()->ApplyImpulse(-(1 / _gameObjects[2]->GetPhysicsModel()->GetMass() * j * collisionNormal));
		//	
		//		Debug::DebugPrintF("Collision");
		//	}
		//	else
		//	{
		//		//Debug::DebugPrintF("No Collision \n");
		//	}
		//	//Debug::DebugPrintF("c.Center.x: %f, c.Center.y: %f, c.Center.z: %f", boxCollider->GetCenter().x, boxCollider->GetCenter().y, boxCollider->GetCenter().z);
		//}
		//else
		//{
		//	Debug::DebugPrintF("No Collision has been added to one of the gameobects");
		//}

		//Debug::DebugPrintF("Gameobject 1 Y position: %f", _gameObjects[1]->GetTransform()->GetPosition().y);
		float angleAroundZ = (_cameraOrbitAngleXZ);

		float x = _cameraOrbitRadius * cos(angleAroundZ);
		float z = _cameraOrbitRadius * sin(angleAroundZ);

		XMFLOAT3 cameraPos = _camera->GetPosition();
		cameraPos.x = x;
		cameraPos.z = z;

		_camera->SetPosition(cameraPos);
		_camera->Update();

		// Update objects
		for (auto gameObject : _gameObjects)
		{
			gameObject->Update(FPS60);
		}
		accumulator -= FPS60;
		_timer.Tick();
	}


	

}

void Application::ResolveCollisions()
{
	CollisionManifold manifold;

	Transform* objectATransform = _gameObjects[1]->GetTransform();
	Transform* objectBTransform = _gameObjects[2]->GetTransform();

	PhysicsModel* objectA = _gameObjects[1]->GetPhysicsModel();
	PhysicsModel* objectB = _gameObjects[2]->GetPhysicsModel();
	PhysicsModel* objectC = _gameObjects[3]->GetPhysicsModel();
	PhysicsModel* objectD = _gameObjects[5]->GetPhysicsModel();
	PhysicsModel* objectE = _gameObjects[6]->GetPhysicsModel();
	

	if (objectA->IsCollideable() && objectB->IsCollideable() && objectA->GetCollider()->CollidesWith(*objectB->GetCollider(),manifold))
	{
		// Get collision normal
		Vector3 collisionNormal = /*objectATransform->GetPosition() - objectBTransform->GetPosition();*/ manifold.collisionNormal;
		// Normalise it
		collisionNormal.Normalize();
		// Get relative velocity
		Vector3 relativeVelocity = objectA->GetVelocity() - objectB->GetVelocity();

		// Dot product
		if (collisionNormal * relativeVelocity < 0.0f)
		{
			// Restitution - Between 0 and 1 for testing
			float e = 1.0f;
			// Get inverse mass of object A
			float invMassA = objectA->GetInverseMass();
			// Get inverse mass of object B
			float invMassB = objectB->GetInverseMass();
			// Calculate the inverse mass sum =  InvMassA + InvMass B
			float inverseMassSum = invMassA + invMassB;
			// Calculate total velocity
			float vj = -(1 + e) * collisionNormal * relativeVelocity;
			// Caluculate impulse
			float j = vj * (inverseMassSum);
		

			
			// Apply Impulse to object A
			objectA->ApplyImpulse(invMassA * j * collisionNormal);
			// Apply Impluse to object B
			objectB->ApplyImpulse(-(invMassB * j * collisionNormal));

			Debug::DebugPrintF("Pen Depth Amount: , %f", manifold.points->penetrationDepth);

			manifold = CollisionManifold();
		}

	}
	if (objectD->IsCollideable() && objectE->IsCollideable() && objectD->GetCollider()->CollidesWith(*objectE->GetCollider(), manifold))
	{
		// Get collision normal
		Vector3 collisionNormal = /*objectATransform->GetPosition() - objectBTransform->GetPosition();*/ manifold.collisionNormal;
		// Normalise it
		collisionNormal.Normalize();
		// Get relative velocity
		Vector3 relativeVelocity = objectD->GetVelocity() - objectE->GetVelocity();

		// Dot product
		if (collisionNormal * relativeVelocity < 0.0f)
		{
			// Restitution - Between 0 and 1 for testing
			float e = 1.0f;
			// Get inverse mass of object A
			float invMassA = objectD->GetInverseMass();
			// Get inverse mass of object B
			float invMassB = objectE->GetInverseMass();
			// Calculate the inverse mass sum =  InvMassA + InvMass B
			float inverseMassSum = invMassA + invMassB;
			// Calculate total velocity
			float vj = -(1 + e) * collisionNormal * relativeVelocity;
			// Caluculate impulse
			float j = vj * (inverseMassSum);


			// Apply Impulse to object A
			objectD->ApplyImpulse(invMassA * j * collisionNormal);
			// Apply Impluse to object B
			objectE->ApplyImpulse(-(invMassB * j * collisionNormal));

			manifold = CollisionManifold();
		}

	}
	if (objectA->IsCollideable() && objectC->IsCollideable() && objectA->GetCollider()->CollidesWith(*objectC->GetCollider(), manifold))
	{
		
		objectA->SetMass(500);
		objectATransform->SetScale(2, 2, 2);
		_pickedUP = true;

		Debug::DebugPrintF("Object A Mass:, %f ", _pickedUP);
		manifold = CollisionManifold();
	}
}

void Application::Draw()
{
    //
    // Clear buffers
    //

	float ClearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f }; // red,green,blue,alpha
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);
	_pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    //
    // Setup buffers and render scene
    //
	_pImmediateContext->IASetInputLayout(_pVertexLayout);

	_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
	_pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);

	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);

    ConstantBuffer cb;

	XMFLOAT4X4 viewAsFloats = _camera->GetView();
	XMFLOAT4X4 projectionAsFloats = _camera->GetProjection();

	XMMATRIX view = XMLoadFloat4x4(&viewAsFloats);
	XMMATRIX projection = XMLoadFloat4x4(&projectionAsFloats);

	cb.View = XMMatrixTranspose(view);
	cb.Projection = XMMatrixTranspose(projection);
	
	cb.light = basicLight;
	cb.EyePosW = _camera->GetPosition();

	// Render all scene objects
	for (auto gameObject : _gameObjects)
	{
		// Get render material
		std::shared_ptr< Material >material = gameObject->GetAppearance()->GetMaterial();

		// Copy material to shader
		cb.surface.AmbientMtrl = material->ambient;
		cb.surface.DiffuseMtrl = material->diffuse;
		cb.surface.SpecularMtrl = material->specular;

		// Set world matrix
		cb.World = XMMatrixTranspose(gameObject->GetTransform()->GetWorldMatrix());

		// Set texture
		if (gameObject->GetAppearance()->HasTexture())
		{
			ID3D11ShaderResourceView * textureRV = gameObject->GetAppearance()->GetTextureRV();
			_pImmediateContext->PSSetShaderResources(0, 1, &textureRV);
			cb.HasTexture = 1.0f;
		}
		else
		{
			cb.HasTexture = 0.0f;
		}

		// Update constant buffer
		_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

		// Draw object
		gameObject->Draw(_pImmediateContext);
	}

	for (auto gameObject : _particleGO)
	{
		// Get render material
		std::shared_ptr< Material >material = gameObject->GetAppearance()->GetMaterial();

		// Copy material to shader
		cb.surface.AmbientMtrl = material->ambient;
		cb.surface.DiffuseMtrl = material->diffuse;
		cb.surface.SpecularMtrl = material->specular;

		// Set world matrix
		cb.World = XMMatrixTranspose(gameObject->GetTransform()->GetWorldMatrix());

		// Set texture
		if (gameObject->GetAppearance()->HasTexture())
		{
			ID3D11ShaderResourceView* textureRV = gameObject->GetAppearance()->GetTextureRV();
			_pImmediateContext->PSSetShaderResources(0, 1, &textureRV);
			cb.HasTexture = 1.0f;
		}
		else
		{
			cb.HasTexture = 0.0f;
		}

		// Update constant buffer
		_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

		// Draw object
		gameObject->Draw(_pImmediateContext);
	}

    //
    // Present our back buffer to our front buffer
    //
    _pSwapChain->Present(0, 0);
}