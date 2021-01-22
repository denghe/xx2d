//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;



// todo: 似乎这里可以不用携带 z, 可以移到 MvpColor 内？
struct Vert {
	XMFLOAT3 pos;
	XMFLOAT2 uv;
};

struct MvpColor {
	XMMATRIX mvp;
	XMFLOAT4 color;
};

ID3D11VertexShader* g_vs = nullptr;
ID3D11PixelShader* g_ps = nullptr;
ID3D11InputLayout* g_il = nullptr;

ID3D11Buffer* g_bufMvpColor = nullptr;
ID3D11ShaderResourceView* g_tex = nullptr;
ID3D11SamplerState* g_ss = nullptr;

// 因为 index 字节长度为 2, 故最大值约为 65536/4 = 16384
// todo: 创建多个 g_bufVerts 以容纳更多顶点. 每帧 cleanup 后将要绘制的对象 idx 无脑追加到临时容器( 追加过程中可能分组，堆排序啥的? ), 最后copy 相关数据到显存( 动态合批 )
size_t g_numVerts = 16384;

ID3D11Buffer* g_bufVerts = nullptr;	// dynamic + cpu write
std::vector<Vert> g_verts;

ID3D11Buffer* g_bufIdxs = nullptr;	// read only 复用
std::vector<WORD> g_idxs;

double fpsTimer = 0;


Game::Game() noexcept :
	m_window(nullptr),
	m_outputWidth(800),
	m_outputHeight(600),
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

	if (kb.Up || kb.W) {
		y += elapsedTime;
	}

	if (kb.Down || kb.S) {
		y -= elapsedTime;
	}

	if (kb.Left || kb.A) {
		x += elapsedTime;
	}

	if (kb.Right || kb.D) {
		x -= elapsedTime;
	}

	if (m_keyboardTracker.pressed.Space) {
		x += 1;
		y += 1;
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

	//if (m.xButton1)
	//{
	//    m_mouse->ResetScrollWheelValue
	//}
	//if (m.xButton2)
	//{
	//}



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


	static XMVECTOR Eye = XMVectorSet(0.0f, 3.0f, -6.0f, 0.0f);
	static XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	static XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	static auto v = XMMatrixLookAtLH(Eye, At, Up);
	static auto p = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_outputWidth / (FLOAT)m_outputHeight, 0.01f, 100.0f);
	static auto vp = v * p;

	//auto&& t = (float)m_timer.GetTotalSeconds();
	//auto m = XMMatrixRotationY(t);

	MvpColor mc;
	mc.mvp = XMMatrixTranspose(/*m * */vp);
	mc.color = { 0.7f, 0.7f, 0.7f, 1.0f };
	m_d3dContext->UpdateSubresource(g_bufMvpColor, 0, nullptr, &mc, 0, 0);


	// 模拟填充 g_verts by Nodes
	g_verts.clear();
	for (size_t i = 0; i < g_numVerts; i++) {
		g_verts.push_back({ { 0,0,0 }, { 0,0 } });
		g_verts.push_back({ { 1,0,0 }, { 1,0 } });
		g_verts.push_back({ { 1,1,0 }, { 1,1 } });
		g_verts.push_back({ { 0,1,0 }, { 0,1 } });
	}

	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	m_d3dContext->Map(g_bufVerts, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	memcpy(d3dMappedResource.pData, g_verts.data(), sizeof(Vert) * g_verts.size());
	m_d3dContext->Unmap(g_bufVerts, 0);

	m_d3dContext->VSSetShader(g_vs, nullptr, 0);
	m_d3dContext->VSSetConstantBuffers(0, 1, &g_bufMvpColor);

	m_d3dContext->PSSetShader(g_ps, nullptr, 0);
	m_d3dContext->PSSetConstantBuffers(0, 1, &g_bufMvpColor);
	m_d3dContext->PSSetShaderResources(0, 1, &g_tex);
	m_d3dContext->PSSetSamplers(0, 1, &g_ss);

	m_d3dContext->DrawIndexed((UINT)g_idxs.size(), 0, 0);

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
	width = 1280;
	height = 720;
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
	//vp.MinDepth = 0.0f;
	//vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_d3dContext->RSSetViewports(1, &vp);

	static std::string shaderCode = R"(
//--------------------------------------------------------------------------------------

Texture2D t : register( t0 );
SamplerState s : register( s0 );

cbuffer cbChangesEveryFrame : register( b0 ) {
    matrix mvp;
    float4 color;
};

//--------------------------------------------------------------------------------------

struct VS_INPUT {
    float4 pos : POSITION;
    float2 uv  : TEXCOORD0;
};

struct PS_INPUT {
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

//--------------------------------------------------------------------------------------

PS_INPUT VS( VS_INPUT input ) {
    PS_INPUT output = (PS_INPUT)0;
    output.pos = mul( input.pos, mvp );
    output.uv = input.uv;
    return output;
}

//--------------------------------------------------------------------------------------

float4 PS( PS_INPUT input) : SV_Target {
    return t.Sample( s, input.uv ) * color;
}
)";

	// Compile the vertex shader
	ID3DBlob* pVSBlob = nullptr;
	DX::ThrowIfFailed(CompileShader(shaderCode, "VS", "vs_4_0", &pVSBlob));

	// Create the vertex shader
	DX::ThrowIfFailed(m_d3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &g_vs));

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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

	D3D11_SUBRESOURCE_DATA InitData = {};

	// Create vertex buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.ByteWidth = (UINT)(sizeof(Vert) * g_numVerts * 4);
	DX::ThrowIfFailed(m_d3dDevice->CreateBuffer(&bd, nullptr, &g_bufVerts));

	// Set vertex buffer
	UINT stride = sizeof(Vert);
	UINT offset = 0;
	m_d3dContext->IASetVertexBuffers(0, 1, &g_bufVerts, &stride, &offset);

	// Create index buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	if (g_idxs.empty()) {
		for (size_t i = 0; i < g_numVerts; i++) {
			auto b = i * 4;
			g_idxs.insert(g_idxs.begin(), {
				(uint16_t)(b + 0)
				, (uint16_t)(b + 2)
				, (uint16_t)(b + 1)
				, (uint16_t)(b + 2)
				, (uint16_t)(b + 0)
				, (uint16_t)(b + 3)
				});
		}
	}
	bd.ByteWidth = (UINT)(sizeof(WORD) * g_idxs.size());
	InitData.pSysMem = g_idxs.data();
	DX::ThrowIfFailed(m_d3dDevice->CreateBuffer(&bd, &InitData, &g_bufIdxs));

	// Set index buffer
	m_d3dContext->IASetIndexBuffer(g_bufIdxs, DXGI_FORMAT_R16_UINT, 0);

	// Set primitive topology
	m_d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffers
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(MvpColor);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	DX::ThrowIfFailed(m_d3dDevice->CreateBuffer(&bd, nullptr, &g_bufMvpColor));

	// Load the Texture
	//DX::ThrowIfFailed(CreateDDSTextureFromFile(m_d3dDevice.Get(), L"seafloor.dds", nullptr, &g_tex));
	DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), m_d3dContext.Get(), L"seafloor.dds", nullptr, &g_tex));

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
}

void Game::OnDeviceLost() {
	if (g_ss) g_ss->Release();
	if (g_tex) g_tex->Release();
	if (g_bufMvpColor) g_bufMvpColor->Release();
	if (g_bufVerts) g_bufVerts->Release();
	if (g_bufIdxs) g_bufIdxs->Release();
	if (g_il) g_il->Release();
	if (g_vs) g_vs->Release();
	if (g_ps) g_ps->Release();

	// TODO: Add Direct3D resource cleanup here.

	m_depthStencilView.Reset();
	m_renderTargetView.Reset();
	m_swapChain.Reset();
	m_d3dContext.Reset();
	m_d3dDevice.Reset();

	CreateDevice();

	CreateResources();
}
