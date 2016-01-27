#ifndef INC_RENDER_SHADERS_H_
#define INC_RENDER_SHADERS_H_

struct CVertexDeclaration;

// ----------------------------------------------
class CVertexShader {
  ID3D11VertexShader*   vs;
  ID3D11InputLayout*    vertex_layout;
public:
  CVertexShader()  : vs( nullptr ), vertex_layout( nullptr ){ }

  bool create(const char* fx_filename
    , const char* entry_point
    , const CVertexDeclaration* vtx_decl
    );
  void destroy();
  void activate();
};

// ----------------------------------------------
class CPixelShader {
  ID3D11PixelShader*   ps;
public:
  CPixelShader() : ps(nullptr) { }

  bool create(const char* fx_filename
    , const char* entry_point
    );
  void destroy();
  void activate();
};



#endif

