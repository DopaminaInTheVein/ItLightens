#include "mcv_platform.h"
#include "windows/app.h"
#include "render/draw_utils.h"
#include "render/render_config.h"

#pragma comment(lib, "d3d11.lib" )
#pragma comment(lib, "d3d9.lib" )

#define MAKE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
const GUID DECLSPEC_SELECTANY name \
                = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }
MAKE_GUID(WKPDID_D3DDebugObjectName, 0x429b8c22, 0x9188, 0x4b0c, 0x87, 0x42, 0xac, 0xb0, 0xbf, 0x85, 0xc2, 0x00);

// The render global obj!
CRender Render;

CRender::CRender()
: swap_chain(nullptr)
, width( 0 )
, height( 0 )
, device(nullptr)
, ctx(nullptr)
, render_target_view( nullptr )
, depth_stencil_view( nullptr )
{ }

void CRender::swapChain() {
  PROFILE_FUNCTION("CRender::swapChain");
  swap_chain->Present(0, 0);
}

// ----------------------------------------
void CRender::activateBackBuffer() {
  ctx->OMSetRenderTargets(1, &render_target_view, depth_stencil_view);
  // Setup the viewport
  D3D11_VIEWPORT vp;
  vp.Width = (FLOAT)width;
  vp.Height = (FLOAT)height;
  vp.MinDepth = 0.0f;
  vp.MaxDepth = 1.0f;
  vp.TopLeftX = 0;
  vp.TopLeftY = 0;
  ctx->RSSetViewports(1, &vp);
}

void CRender::clearMainZBuffer() {
  ctx->ClearDepthStencilView(depth_stencil_view, D3D11_CLEAR_DEPTH, 1.0f, 0);
  //ctx->ClearDepthStencilView(depth_stencil_view, D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void CRender::destroyDevice() {
  destroyRenderStateConfigs();
  SAFE_RELEASE(render_target_view);
  SAFE_RELEASE(depth_stencil_view);
  SAFE_RELEASE(depth_resource);
  if (ctx) ctx->ClearState();
  if (swap_chain) swap_chain->Release(), swap_chain = nullptr;
  if (ctx) ctx->Release(), ctx = nullptr;
  if (device) device->Release(), device = nullptr;
}

bool CRender::createDevice() {

	CApp& app = CApp::get();

	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(app.getHWnd(), &rc);
	width = rc.right - rc.left;
	height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = app.getHWnd();
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	hr = D3D11CreateDeviceAndSwapChain(NULL, 
		D3D_DRIVER_TYPE_HARDWARE, NULL, 
		createDeviceFlags, 
		featureLevels, 
		numFeatureLevels,
		D3D11_SDK_VERSION, 
		&sd, &swap_chain, &device, &featureLevel, &ctx);
	if (!SUCCEEDED(hr))
		return false;
	setDXName(device, "DX11Dev");

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
		return false;

	hr = device->CreateRenderTargetView(pBackBuffer, NULL, &render_target_view);
	pBackBuffer->Release();
	if (FAILED(hr))
		return false;
	setDXName(pBackBuffer, "BackBufferRes");
	setDXName(render_target_view, "BackBufferRTV");

	if (!createDepthBuffer(width, height, DXGI_FORMAT_D24_UNORM_S8_UINT, &depth_resource, &depth_stencil_view, "ZBackBuffer"))
		return false;
	setDXName(depth_resource, "MainZBufferRes");
	setDXName(depth_stencil_view, "MainZBufferDSV");

  activateBackBuffer();

  createRenderStateConfigs();

  dbg("Render.device created\n");

	return true;
}

