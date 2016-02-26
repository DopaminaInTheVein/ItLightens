#ifndef INC_RENDER_H_
#define INC_RENDER_H_

#include <d3d11.h>

class CRender {
	IDXGISwapChain*         swap_chain;
	D3D_FEATURE_LEVEL       featureLevel;

public:
	CRender();

	// Remove copy ctor
	CRender(const CRender&) = delete;

	ID3D11Device*           device;
	ID3D11DeviceContext*    ctx;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11Texture2D*        depthStencil;
	ID3D11DepthStencilView* zbuffer;

	void destroyDevice();
	bool createDevice();
	void swapChain();

	bool createZBuffer();
};

extern CRender Render;
//#define Render		CRender::get()

//#ifdef NDEBUG
//#define setDXName(dx_obj,new_name)
//#else
#define setDXName(dx_obj,new_name) \
        dx_obj->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)strlen(new_name), new_name);
//#endif

#include "gpu_trace.h"

#endif
