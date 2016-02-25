#ifndef INC_RENDER_SHADERS_H_
#define INC_RENDER_SHADERS_H_

#include "resources/resource.h"

struct CVertexDeclaration;

// ----------------------------------------------
class CVertexShader : public IResource {
	ID3D11VertexShader*   vs;
	ID3D11InputLayout*    vertex_layout;
public:
	CVertexShader() : vs(nullptr), vertex_layout(nullptr) { }

	bool create(const char* fx_filename
		, const char* entry_point
		, const CVertexDeclaration* vtx_decl
		);
	void destroy() override;
	void activate() const;

	bool isValid() const override {
		return vs != nullptr;
	}
	eType getType() const override {
		return VERTEX_SHADER;
	}
};

// ----------------------------------------------
class CPixelShader : public IResource {
	ID3D11PixelShader*   ps;
public:
	CPixelShader() : ps(nullptr) { }

	bool create(const char* fx_filename
		, const char* entry_point
		);
	void destroy() override;
	void activate() const;

	bool isValid() const override {
		return ps != nullptr;
	}
	eType getType() const override {
		return PIXEL_SHADER;
	}
};

#endif
