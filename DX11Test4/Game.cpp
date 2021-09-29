//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;


//struct QuadData_center_scale_rotate {
//	int16_t x;
//	int16_t y;
//	int16_t scale;
//	int16_t rotate;
//};
//std::vector<QuadData_center_scale_rotate> quads;

struct QuadData_vert {
	uint16_t x;
	uint16_t y;
};
QuadData_vert verts[4] = {
{ 0, 0 },
{ 0, 65535 },
{ 65535, 0 },
{ 65535, 65535 },
};

struct QuadData_center_scale_rotate {
	XMFLOAT4 csr;
};
std::vector<QuadData_center_scale_rotate> quads;

//struct QuadData_vert {
//	XMFLOAT2 uv;
//};
//QuadData_vert verts[4] = {
//XMFLOAT2{ 0, 0 },
//XMFLOAT2{ 0, 1 },
//XMFLOAT2{ 1, 0 },
//XMFLOAT2{ 1, 1 },
//};

ID3D11VertexShader* g_vs = nullptr;
ID3D11PixelShader* g_ps = nullptr;
ID3D11InputLayout* g_il = nullptr;

ID3D11ShaderResourceView* g_tex = nullptr;
ID3D11SamplerState* g_ss = nullptr;

ID3D11Buffer* g_bufInstances = nullptr;	// dynamic + cpu write
ID3D11Buffer* g_bufVerts = nullptr;		// read only ∏¥”√
ID3D11Buffer* g_bufProj = nullptr;		// read only ∏¥”√


double fpsTimer = 0;


Game::Game() noexcept :
	m_window(nullptr),
	m_outputWidth(1920),
	m_outputHeight(1080),
	m_featureLevel(D3D_FEATURE_LEVEL_9_1) {
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height) {
	m_window = window;
	m_outputWidth = std::max(width, 1);
	m_outputHeight = std::max(height, 1);

	m_fullScreen = false;

	m_keyboard = std::make_unique<Keyboard>();
	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetWindow(window);

	CreateDevice();

	CreateResources();

	// TODO: Change the timer settings if you want something other than the default variable timestep mode.
	// e.g. for 60 FPS fixed timestep update logic, call:
	/*
	m_timer.SetFixedTimeStep(true);
	m_timer.SetTargetElapsedSeconds(1.0 / 60);
	*/

	auto random_of_range = [](float min, float max) {
		return min + (max - min) * (float(rand()) / RAND_MAX);
	};

	int n = 1000000;
	quads.resize(n);
	for (auto& q : quads) {
		q.csr.x = random_of_range(0, width);
		q.csr.y = random_of_range(0, height);
		q.csr.z = 128;
		q.csr.w = 0;
		//q.x = random_of_range(0, width);
		//q.y = random_of_range(0, height);
		//q.scale = 128;
		//q.rotate = 0;
	}
}

// Executes the basic game loop.
void Game::Tick() {
	m_timer.Tick([&]() {
		Update(m_timer);
		});

	Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer) {
	float elapsedTime = float(timer.GetElapsedSeconds());
	++counter;
	if (!m_fullScreen) {
		auto t = m_timer.GetTotalSeconds();
		if (t > fpsTimer) {
			fpsTimer = t + 1;
			wchar_t buf[32];
			wsprintf(buf, L"%d", int(counter / t));
			SetWindowText(m_window, buf);
		}
	}

	auto kb = m_keyboard->GetState();
	m_keyboardTracker.Update(kb);

	if (kb.Escape) {
		ExitGame();
	}


	auto m = m_mouse->GetState();
	//if (!m_mouse->IsConnected()) ...
	m_mouseTracker.Update(m);

	if (m.leftButton) {
		z += elapsedTime;
	}
	if (m.rightButton) {
		z -= elapsedTime;
	}

	if (m_mouseTracker.middleButton == m_mouseTracker.PRESSED) {
		m_mouse->SetMode(Mouse::MODE_RELATIVE); // system cursor is not visible
	}

	if (m_mouseTracker.middleButton == m_mouseTracker.RELEASED) {
		m_mouse->SetMode(Mouse::MODE_ABSOLUTE); // system cursor is visible 
	}

	// TODO: Add your game logic here.
	elapsedTime;
}

// Draws the scene.
void Game::Render() {
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
		return;

	Clear();

	// TODO: Add your rendering code here.

	// fill global buf( u_projection )
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	m_d3dContext->Map(g_bufProj, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	{
		auto& d = *(XMFLOAT4*)d3dMappedResource.pData;
		d.x = 2.0f / m_outputWidth;
		d.y = 2.0f / m_outputHeight;
		d.z = 0.5f;//-1.0f;
		d.w = -1.0f;
	}
	m_d3dContext->Unmap(g_bufProj, 0);

	// fill instance buf
	m_d3dContext->Map(g_bufInstances, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	memcpy(d3dMappedResource.pData, quads.data(), sizeof(QuadData_center_scale_rotate) * quads.size());
	m_d3dContext->Unmap(g_bufInstances, 0);

	m_d3dContext->VSSetConstantBuffers(0, 1, &g_bufProj);
	m_d3dContext->PSSetShaderResources(0, 1, &g_tex);
	m_d3dContext->PSSetSamplers(0, 1, &g_ss);

	m_d3dContext->VSSetShader(g_vs, nullptr, 0);
	m_d3dContext->PSSetShader(g_ps, nullptr, 0);
	m_d3dContext->DrawInstanced(_countof(verts), (UINT)quads.size(), 0, 0);

	Present();
}

// Helper method to clear the back buffers.
void Game::Clear() {
	// Clear the views.
	m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), Colors::CornflowerBlue);
	m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

	// Set the viewport.
	CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(m_outputWidth), static_cast<float>(m_outputHeight));
	m_d3dContext->RSSetViewports(1, &viewport);
}

// Presents the back buffer contents to the screen.
void Game::Present() {
	// The first argument instructs DXGI to block until VSync, putting the application
	// to sleep until the next VSync. This ensures we don't waste any cycles rendering
	// frames that will never be displayed to the screen.
	HRESULT hr = m_swapChain->Present(0, 0);

	// If the device was reset we must completely reinitialize the renderer.
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
		OnDeviceLost();
	}
	else {
		DX::ThrowIfFailed(hr);
	}
}

// Message handlers
void Game::OnActivated() {
	// TODO: Game is becoming active window.
}

void Game::OnDeactivated() {
	// TODO: Game is becoming background window.
}

void Game::OnSuspending() {
	// TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming() {
	m_timer.ResetElapsedTime();

	// TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowSizeChanged(int width, int height) {
	width = std::max(width, 1);
	height = std::max(height, 1);
	if (m_outputWidth != width || m_outputHeight != height) {
		m_outputWidth = width;
		m_outputHeight = height;

		CreateResources();
		// TODO: Game window is being resized.
	}
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept {
	// TODO: Change to desired default window size (note minimum size is 320x200).
	width = 1920;
	height = 1080;
}

// These are the resources that depend on the device.
void Game::CreateDevice() {
	UINT creationFlags = 0;

#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	static const D3D_FEATURE_LEVEL featureLevels[] =
	{
		// TODO: Modify for supported Direct3D feature levels
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};

	// Create the DX11 API device object, and get a corresponding context.
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;
	DX::ThrowIfFailed(D3D11CreateDevice(
		nullptr,                            // specify nullptr to use the default adapter
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		creationFlags,
		featureLevels,
		_countof(featureLevels),
		D3D11_SDK_VERSION,
		device.ReleaseAndGetAddressOf(),    // returns the Direct3D device created
		&m_featureLevel,                    // returns feature level of device created
		context.ReleaseAndGetAddressOf()    // returns the device immediate context
	));

#ifndef NDEBUG
	ComPtr<ID3D11Debug> d3dDebug;
	if (SUCCEEDED(device.As(&d3dDebug))) {
		ComPtr<ID3D11InfoQueue> d3dInfoQueue;
		if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue))) {
#ifdef _DEBUG
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
			D3D11_MESSAGE_ID hide[] = {
				D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
				// TODO: Add more message IDs here as needed.
			};
			D3D11_INFO_QUEUE_FILTER filter = {};
			filter.DenyList.NumIDs = _countof(hide);
			filter.DenyList.pIDList = hide;
			d3dInfoQueue->AddStorageFilterEntries(&filter);
		}
	}
#endif

	DX::ThrowIfFailed(device.As(&m_d3dDevice));
	DX::ThrowIfFailed(context.As(&m_d3dContext));

	// TODO: Initialize device dependent objects here (independent of window size).
}






HRESULT CompileShader(std::string code, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut) {
	HRESULT hr = S_OK;
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif
	ID3DBlob* pErrorBlob;
	hr = D3DCompile(code.data(), code.size(), nullptr, nullptr, nullptr, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
	if (pErrorBlob) {
		OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		pErrorBlob->Release();
	}
	return hr;
}


// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateResources() {
	// Clear the previous window size specific context.
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	m_d3dContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
	m_renderTargetView.Reset();
	m_depthStencilView.Reset();
	m_blendState.Reset();
	m_d3dContext->Flush();

	const UINT backBufferWidth = static_cast<UINT>(m_outputWidth);
	const UINT backBufferHeight = static_cast<UINT>(m_outputHeight);
	const DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
	const DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	constexpr UINT backBufferCount = 2;

	// If the swap chain already exists, resize it, otherwise create one.
	if (m_swapChain) {
		HRESULT hr = m_swapChain->ResizeBuffers(backBufferCount, backBufferWidth, backBufferHeight, backBufferFormat, 0);

		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
			// If the device was removed for any reason, a new device and swap chain will need to be created.
			OnDeviceLost();

			// Everything is set up now. Do not continue execution of this method. OnDeviceLost will reenter this method 
			// and correctly set up the new device.
			return;
		}
		else {
			DX::ThrowIfFailed(hr);
		}
	}
	else {
		// First, retrieve the underlying DXGI Device from the D3D Device.
		ComPtr<IDXGIDevice2> dxgiDevice;
		DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

		// Identify the physical adapter (GPU or card) this device is running on.
		ComPtr<IDXGIAdapter4> dxgiAdapter;
		DX::ThrowIfFailed(dxgiDevice->GetAdapter((IDXGIAdapter**)dxgiAdapter.GetAddressOf()));

		// And obtain the factory object that created it.
		ComPtr<IDXGIFactory2> dxgiFactory;
		DX::ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

		// Create a descriptor for the swap chain.
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = backBufferWidth;
		swapChainDesc.Height = backBufferHeight;
		swapChainDesc.Format = backBufferFormat;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = backBufferCount;
		//swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
		fsSwapChainDesc.Windowed = TRUE;

		// Create a SwapChain from a Win32 window.
		DX::ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
			m_d3dDevice.Get(),
			m_window,
			&swapChainDesc,
			&fsSwapChainDesc,
			nullptr,
			m_swapChain.ReleaseAndGetAddressOf()
		));

		// This template does not support exclusive fullscreen mode and prevents DXGI from responding to the ALT+ENTER shortcut.
		DX::ThrowIfFailed(dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER));
	}

	// Obtain the backbuffer for this window which will be the final 3D rendertarget.
	ComPtr<ID3D11Texture2D> backBuffer;
	DX::ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));

	// Create a view interface on the rendertarget to use on bind.
	DX::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.ReleaseAndGetAddressOf()));

	// Allocate a 2-D surface as the depth/stencil buffer and
	// create a DepthStencil view on this surface to use on bind.
	CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL);

	ComPtr<ID3D11Texture2D> depthStencil;
	DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()));

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, m_depthStencilView.ReleaseAndGetAddressOf()));

	// TODO: Initialize windows-size dependent objects here.

	// Setup the viewport
	D3D11_VIEWPORT vp;
	ZeroMemory(&vp, sizeof(D3D11_VIEWPORT));
	vp.Width = (FLOAT)m_outputWidth;
	vp.Height = (FLOAT)m_outputHeight;
	//vp.MinDepth = 0;
	//vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_d3dContext->RSSetViewports(1, &vp);

	static std::string shaderCode = R"(
Texture2D t : register(t0);
SamplerState s : register(s0);
cbuffer BufProj : register(b0) {
	float4 u_projection;
};

struct VS_INPUT {
	float2 vert					: TEXCOORD0;
	float4 center_scale_rotate	: TEXCOORD1;
};

struct PS_INPUT {
	float4 pos		: SV_POSITION;
	float2 texcoord : TEXCOORD0;
};

PS_INPUT VS( VS_INPUT i ) {
	PS_INPUT o;
    float2 offset = float2(i.vert.x - 0.5, i.vert.y - 0.5);
    float2 texcoord = i.vert;
    float2 center = i.center_scale_rotate.xy;
    float scale = i.center_scale_rotate.z;
    float rotate = i.center_scale_rotate.w;

    float cos_theta = cos(rotate);
    float sin_theta = sin(rotate);
    float2 v1 = offset * scale;
    float2 v2 = float2(
       dot(v1, float2(cos_theta, sin_theta)),
       dot(v1, float2(-sin_theta, cos_theta))
    );
    float2 v3 = (v2 + center) * u_projection.xy + u_projection.zw;

    o.texcoord = texcoord;
    o.pos = float4(v3.x, v3.y, 0.0, 1.0);
	return o;
}

float4 PS( PS_INPUT i ) : SV_Target {
	return t.Sample(s, i.texcoord);
}
)";

	// Compile the vertex shader
	ID3DBlob* pVSBlob = nullptr;
	DX::ThrowIfFailed(CompileShader(shaderCode, "VS", "vs_4_0", &pVSBlob));

	// Create the vertex shader
	DX::ThrowIfFailed(m_d3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &g_vs));

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "TEXCOORD", 0, DXGI_FORMAT_R16G16_UNORM, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	DX::ThrowIfFailed(m_d3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &g_il));
	pVSBlob->Release();

	// Set the input layout
	m_d3dContext->IASetInputLayout(g_il);

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
	DX::ThrowIfFailed(CompileShader(shaderCode, "PS", "ps_4_0", &pPSBlob));

	// Create the pixel shader
	DX::ThrowIfFailed(m_d3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_ps));
	pPSBlob->Release();

	// Prepare
	D3D11_BUFFER_DESC bd = {};

	// Create instance buffer
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	bd.ByteWidth = sizeof(QuadData_center_scale_rotate) * 5000000;
	DX::ThrowIfFailed(m_d3dDevice->CreateBuffer(&bd, nullptr, &g_bufInstances));

	// Create const buffer ( float4 u_projection )
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	bd.ByteWidth = sizeof(XMFLOAT4);
	DX::ThrowIfFailed(m_d3dDevice->CreateBuffer(&bd, nullptr, &g_bufProj));

	// Create vertex buffer & fill
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	bd.ByteWidth = sizeof(verts);
	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = verts;
	DX::ThrowIfFailed(m_d3dDevice->CreateBuffer(&bd, &InitData, &g_bufVerts));

	// Set buffers
	unsigned int strides[2];
	unsigned int offsets[2];
	ID3D11Buffer* bufferPointers[2];
	strides[0] = sizeof(QuadData_vert);
	strides[1] = sizeof(QuadData_center_scale_rotate);
	offsets[0] = 0;
	offsets[1] = 0;
	bufferPointers[0] = g_bufVerts;
	bufferPointers[1] = g_bufInstances;
	m_d3dContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

	// Set primitive topology
	m_d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// Load the Texture
	DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), m_d3dContext.Get(), L"../../res/b.png", nullptr, &g_tex));

	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	DX::ThrowIfFailed(m_d3dDevice->CreateSamplerState(&sampDesc, &g_ss));

	// Create blend state
	D3D11_BLEND_DESC blendStateDescription = {};
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	DX::ThrowIfFailed(m_d3dDevice->CreateBlendState(&blendStateDescription, &m_blendState));

	// Set defalt blend state for alpha display
	float blendFactor[] = { 0.f,0.f,0.f,0.f };
	m_d3dContext->OMSetBlendState(m_blendState.Get(), blendFactor, 0xffffffff);
}

void Game::OnDeviceLost() {
	if (g_ss) g_ss->Release();
	if (g_tex) g_tex->Release();
	if (g_bufProj) g_bufProj->Release();
	if (g_bufInstances) g_bufInstances->Release();
	if (g_bufVerts) g_bufVerts->Release();
	if (g_il) g_il->Release();
	if (g_vs) g_vs->Release();
	if (g_ps) g_ps->Release();

	// TODO: Add Direct3D resource cleanup here.

	m_blendState.Reset();
	m_depthStencilView.Reset();
	m_renderTargetView.Reset();
	m_swapChain.Reset();
	m_d3dContext.Reset();
	m_d3dDevice.Reset();

	CreateDevice();

	CreateResources();
}
