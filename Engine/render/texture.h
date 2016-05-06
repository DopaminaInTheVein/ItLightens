#ifndef INC_RENDER_TEXTURE_H_
#define INC_RENDER_TEXTURE_H_

#include "resources/resource.h"

class CTexture : public IResource {
protected:
  ID3D11Texture2D*          resource;
  ID3D11ShaderResourceView* res_view;
  int                       xres, yres;

public:

  enum TCreateOptions {
    CREATE_STATIC
  , CREATE_DYNAMIC
  , CREATE_RENDER_TARGET
  };

  void renderUIDebug();

  CTexture( ) 
  : resource( nullptr )
  , res_view( nullptr )
  , xres(0)
  , yres(0)
  { }

  bool isValid() const override {
    return res_view != nullptr;
  }
  void destroy() override {
    SAFE_RELEASE(resource);
    SAFE_RELEASE(res_view);
  }
  eType getType() const override { return TEXTURE; }

  bool load(const char* filename);

  bool loadFullPath(const char * filename);

  void activate(int slot) const;
  static void deactivate(int slot);
  bool reload() override;

bool create(
    int nxres
    , int nyres
    , DXGI_FORMAT nformat
    , TCreateOptions options);

  // -----------------------------------------------
  void setDXObjs(
    ID3D11Texture2D*          new_resource
  , ID3D11ShaderResourceView* new_res_view
  ) {
    resource = new_resource;
    res_view = new_res_view;
  }

};

#endif
