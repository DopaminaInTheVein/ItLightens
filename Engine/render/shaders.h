#ifndef INC_RENDER_SHADERS_H_
#define INC_RENDER_SHADERS_H_

#include "resources/resource.h"

struct CVertexDeclaration;

// ----------------------------------------------
class CVertexShader : public IResource {
  std::string           src_fx_filename;
  std::string           entry_point;
  ID3D11VertexShader*   vs;
  ID3D11InputLayout*    vertex_layout;
  const CVertexDeclaration*   vtx_decl;
public:
  CVertexShader()  : vs( nullptr ), vertex_layout( nullptr ), vtx_decl( nullptr ) { }

  bool create(const char* fx_filename
    , const char* entry_point
    , const CVertexDeclaration* vtx_decl
    );
  void destroy() override;
  void activate() const;

  bool isValid() const override {
    return vs != nullptr;
  }
  eType getType() const override  {
    return VERTEX_SHADER;
  }
  void onFileChanged(const std::string& filename);
  const CVertexDeclaration* getVertexDecl() const { return vtx_decl; }
};

// ----------------------------------------------
class CPixelShader : public IResource {
  std::string           src_fx_filename;
  std::string           entry_point;
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
  void onFileChanged(const std::string& filename);
};



#endif

