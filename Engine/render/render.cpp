#include "mcv_platform.h"
#include "windows/app.h"

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
	, device(nullptr)
	, ctx(nullptr)
	, renderTargetView(nullptr)
	, zbuffer(nullptr)
	, depthStencil(nullptr)
{ }

void CRender::swapChain() {
	PROFILE_FUNCTION("swap");
	swap_chain->Present(0, 0);
}

bool CRender::createZBuffer()
{
	CApp& app = CApp::get();

	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(app.getHWnd(), &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = device->CreateTexture2D(&descDepth, NULL, &depthStencil);
	if (FAILED(hr)) { return false; }


	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = device->CreateDepthStencilView(depthStencil, &descDSV, &zbuffer);
	if (FAILED(hr))
		return false;

	return true;
}

void CRender::destroyDevice() {
	if (ctx) ctx->ClearState();
	if (renderTargetView) renderTargetView->Release(), renderTargetView = nullptr;
	if (swap_chain) swap_chain->Release(), swap_chain = nullptr;
	if (ctx) ctx->Release(), ctx = nullptr;
	if (device) device->Release(), device = nullptr;
	if (zbuffer) zbuffer->Release(), zbuffer = nullptr;
	if (depthStencil) depthStencil->Release(), depthStencil = nullptr;
}

bool CRender::createDevice() {
	CApp& app = CApp::get();

	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(app.getHWnd(), &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

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

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
		return false;

	hr = device->CreateRenderTargetView(pBackBuffer, NULL, &renderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
		return false;

	//zBuffer:
	if (createZBuffer())
		dbg("zbuffer created\n");

	ctx->OMSetRenderTargets(1, &renderTargetView, zbuffer);

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	ctx->RSSetViewports(1, &vp);

	dbg("Render.device created\n");

	return true;
}