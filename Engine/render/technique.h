#ifndef INC_RENDER_TECHNIQUE_H_
#define INC_RENDER_TECHNIQUE_H_

#include "render/render.h"
#include "resources/resource.h"
#include "utils/XMLParser.h"

struct CVertexDeclaration;
class  CVertexShader;
class  CHandle;
class  CPixelShader;

class CRenderTechnique : public IResource, public CXMLParser {

public:
  static const CRenderTechnique* curr_active;
  static const CVertexDeclaration* getCurrentVertexDecl();

  CRenderTechnique() : vs(nullptr), ps(nullptr), priority( 100 ) { }
  CRenderTechnique(const CRenderTechnique&) = delete;

  void destroy();
  void activate() const;

  // -----------------------------------------------------
  enum eCategory {
    SOLID_OBJS
  , DETAIL_OBJS
  , TRANSPARENT_OBJS
  , DBG_OBJS
  , UI_OBJS
  };
  eCategory getCategory() const { return category; }

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

public:
	float getPriority() const {
		return priority;
	}

	float getPriority(CHandle) const;
};

#endif

