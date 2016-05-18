#ifndef INC_RENDER_H_
#define INC_RENDER_H_

#include <d3d11.h>

class CRenderTechnique;
class CTexture;
class CMesh;

class CRender {
	IDXGISwapChain*         swap_chain;
	D3D_FEATURE_LEVEL       featureLevel;
  int                     width, height;
  
public:
	CRender();

  // Remove copy ctor
  CRender(const CRender&) = delete;

	ID3D11Device*           device;
	ID3D11DeviceContext*    ctx;
  ID3D11RenderTargetView* render_target_view;
  
  ID3D11Texture2D*        depth_resource;
  ID3D11DepthStencilView* depth_stencil_view;

  void destroyDevice();
	bool createDevice();
	void swapChain();

  void activateBackBuffer();
  void clearMainZBuffer();
};

extern CRender Render;
//#define Render		CRender::get()

//#ifdef NDEBUG
//#define setDXName(dx_obj,new_name) 
//#else
#define setDXName(dx_obj,new_name) \
        dbg( "DXObj %p will have name %s\n", dx_obj, new_name ); \
        dx_obj->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)strlen(new_name), new_name);
//#endif

#include "gpu_trace.h"

#include "mesh.h"
#include "texture.h"
#include "render_to_texture.h"
#include "technique.h"
#include "render_manager.h"
#include "material.h"
#include "render_config.h"

#endif

