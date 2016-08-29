#include "mcv_platform.h"
#include "technique.h"
#include "resources/resources_manager.h"
#include "shaders.h"
#include "render/vertex_declarations.h"
#include "handle\handle.h"
#include "components\entity.h"
#include "components\comp_transform.h"

template<> IResource::eType getTypeOfResource<CRenderTechnique>() { return IResource::TECHNIQUE; }

// name = techniques/tech_solid_colored.tech
template<>
IResource* createObjFromName<CRenderTechnique>(const std::string& name) {
  CRenderTechnique* tech = new CRenderTechnique;
  std::string full_path = "data/shaders/" + name;
  bool is_ok = tech->xmlParseFile(full_path.c_str());
  assert(is_ok);
  tech->setName(name);
  return tech;
}

const CRenderTechnique* CRenderTechnique::curr_active = nullptr;

// --------------------------------
void CRenderTechnique::onStartElement(const std::string &elem, MKeyValue &atts) {
  if (elem == "vs") {
    auto fx = atts["fx"];       // "basic"
    auto main = atts["main"];   // "VS"
    auto decl_name = atts["decl"];   // "positions_color"
    auto res_name = main + "@" + fx + ".vs";
    fx = "data/shaders/" + fx + ".fx";
    if (!Resources.isValid(res_name.c_str())) {
      CVertexShader* vs = new CVertexShader;
      auto vdecl = vdecl_manager.getByName(decl_name);
      vs->create(fx.c_str(), main.c_str(), vdecl);
      vs->setName(res_name);
      Resources.registerNew(vs);
    }
    vs = Resources.get(res_name.c_str() )->as<CVertexShader>();
  }
  else if (elem == "ps") {
    assert(!ps_disabled);
    auto fx = atts["fx"];       // "basic"
    auto main = atts["main"];   // "PS"
    auto res_name = main + "@" + fx + ".ps";
    fx = "data/shaders/" + fx + ".fx";
    if (!Resources.isValid(res_name.c_str())) {
      CPixelShader* ps = new CPixelShader;
      ps->create(fx.c_str(), main.c_str());
      ps->setName(res_name);
      Resources.registerNew(ps);
    }
    ps = Resources.get(res_name.c_str())->as<CPixelShader>();
  }
  else if (elem == "priority") {
	  priority = atts.getFloat("value", 100);
  }
  else if (elem == "tech") {
    uses_bones = atts.getBool("uses_bones", false);

    category = SOLID_OBJS;
    if (atts.getBool("is_transparent", false))
      category = TRANSPARENT_OBJS;
    if (atts.getBool("is_ui", false))
      category = UI_OBJS;
    if (atts.getBool("is_dbg", false))
      category = DBG_OBJS;
	if (atts.getBool("is_details", false))
		category = DETAIL_OBJS;

    ps_disabled = atts.getBool( "ps_disabled", false );
  }
}

float CRenderTechnique::getPriority(CHandle owner) const
{
	if (category == UI_OBJS) {
		CEntity* e_owner = owner;
		if (!e_owner) return priority;

		TCompTransform* trans = e_owner->get<TCompTransform>();
		if (!trans) return priority;

		return trans->getPosition().z;
	}
	return priority;
}

void CRenderTechnique::destroy() {
  vs = nullptr;
  ps = nullptr;
}

const CVertexDeclaration* CRenderTechnique::getCurrentVertexDecl() {
  assert(curr_active);
  return curr_active->vs->getVertexDecl();
}

void CRenderTechnique::activate() const {
  curr_active = this;
  assert(isValid());
  if( ps )
    ps->activate();
  vs->activate();
}

