#ifndef INC_RENDER_TECHNIQUE_H_
#define INC_RENDER_TECHNIQUE_H_

#include "render/render.h"
#include "resources/resource.h"
#include "utils/XMLParser.h"

struct CVertexDeclaration;
class  CVertexShader;
class  CPixelShader;

class CRenderTechnique : public IResource, public CXMLParser {

  const CVertexShader* vs;
  const CPixelShader*  ps;
  std::string          name;
  int                  priority;

  static CRenderTechnique* curr_active;

  void onStartElement(const std::string &elem, MKeyValue &atts) override;

public:

  CRenderTechnique() : vs(nullptr), ps(nullptr), priority( 100 ) { }
  CRenderTechnique(const CRenderTechnique&) = delete;

  void destroy();
  bool create(const char* new_name, const CVertexShader* new_vs, const CPixelShader* new_ps);

  void activate() const;

  int getPriority() const { return priority; }

  const std::string& getName() const {
    return name;
  }

  bool isValid() const {
    return vs && ps;
  }

  eType getType() const { return TECHNIQUE; }
};

#endif

