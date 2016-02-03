#ifndef INC_RENDER_TECHNIQUE_H_
#define INC_RENDER_TECHNIQUE_H_

#include "render/render.h"
#include "resources/resource.h"


struct CVertexDeclaration;
class  CVertexShader;
class  CPixelShader;

class CRenderTechnique : public IResource {

  const CVertexShader* vs;
  const CPixelShader*  ps;
  std::string          name;

public:

  CRenderTechnique() : vs(nullptr), ps(nullptr) { }
  CRenderTechnique(const CRenderTechnique&) = delete;

  void destroy();
  bool create(const char* new_name, const CVertexShader* new_vs, const CPixelShader* new_ps);

  void activate() const;
  void render() const;

  const std::string& getName() const {
    return name;
  }

  bool isValid() const {
    return vs && ps;
  }

  eType getType() const { return TECHNIQUE; }
  //void renderUIDebug() override;
};

#endif

