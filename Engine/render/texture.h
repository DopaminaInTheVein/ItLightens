#ifndef INC_RENDER_TEXTURE_H_
#define INC_RENDER_TEXTURE_H_

class CTexture : public IResource {
	ID3D11Resource*           resource;
	ID3D11ShaderResourceView* res_view;
public:

  void renderUIDebug();


  CTexture( ) : resource( nullptr ), res_view( nullptr ) 
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

  bool reload() override;

};

#endif
