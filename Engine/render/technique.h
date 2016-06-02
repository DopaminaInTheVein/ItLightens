#ifndef INC_RENDER_TECHNIQUE_H_
#define INC_RENDER_TECHNIQUE_H_

#include "render/render.h"
#include "resources/resource.h"

struct CVertexDeclaration;
class  CVertexShader;
class  CPixelShader;

class CRenderTechnique : public IResource, public CXMLParser {
<<<<<<< HEAD
  const CVertexShader* vs;
  const CPixelShader*  ps;
  std::string          name;
  int                  priority;
  bool                 uses_bones;
  bool                 is_transparent;
  bool                 ps_disabled;

  void onStartElement(const std::string &elem, MKeyValue &atts) override;
=======
>>>>>>> d6b4e6803fa82f01d5c091b986f30dbebbb8b427

public:
  static const CRenderTechnique* curr_active;
  static const CVertexDeclaration* getCurrentVertexDecl();

  CRenderTechnique() : vs(nullptr), ps(nullptr), priority(100) { }
  CRenderTechnique(const CRenderTechnique&) = delete;

  void destroy();
  void activate() const;

  // -----------------------------------------------------
  enum eCategory {
    SOLID_OBJS
  , TRANSPARENT_OBJS
  , DBG_OBJS
  , UI_OBJS
  };
  eCategory getCategory() const { return category; }
  int getPriority() const { return priority; }
  bool isTransparent() const { return is_transparent; }

  const std::string& getName() const {
    return name;
  }

  bool isValid() const {
    return vs && ( ps || ps_disabled );
  }

  bool usesBones() const { return uses_bones; }

  eType getType() const { return TECHNIQUE; }

private:

  const CVertexShader* vs;
  const CPixelShader*  ps;
  std::string          name;
  eCategory            category;
  int                  priority;
  bool                 uses_bones;
  bool                 ps_disabled;

  void onStartElement(const std::string &elem, MKeyValue &atts) override;

};

#endif
