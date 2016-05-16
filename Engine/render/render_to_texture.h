#ifndef INC_RENDER_RENDER_TO_TEXTURE_H_
#define INC_RENDER_RENDER_TO_TEXTURE_H_

#include "texture.h"
#include <d3d11.h>

class CRenderToTexture : public CTexture {
	DXGI_FORMAT             color_format;
	DXGI_FORMAT             depth_format;

	ID3D11RenderTargetView* render_target_view;
	ID3D11DepthStencilView* depth_stencil_view;

	ID3D11Texture2D*        depth_resource;

public:

	CRenderToTexture();
	~CRenderToTexture();

	bool createRT(const char* name, int new_xres, int new_yres
		, DXGI_FORMAT new_color_format
		, DXGI_FORMAT new_depth_format
		);
	void destroy() override;

	void activateRT();
	void activateViewport();

	void clear(VEC4 clear_color);
	void clearZ();

	ID3D11RenderTargetView* getRenderTargetView() {
		return render_target_view;
	}
};

#endif
