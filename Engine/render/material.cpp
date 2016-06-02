#include "mcv_platform.h"
#include "resources/resources_manager.h"
#include "material.h"
#include "texture.h"
#include "constants/ctes_platform.h"
#include "draw_utils.h"

template<> IResource::eType getTypeOfResource<CMaterial>() { return IResource::MATERIAL; }

template<>
IResource* createObjFromName<CMaterial>(const std::string& name) {
	CMaterial* obj = new CMaterial;
	if (!obj->load(name.c_str())) {
		dbg("Can't load CMaterial %s. Will try placeholder...\n", name.c_str());
	}
	obj->setName(name.c_str());
	return obj;
}

void CMaterial::renderUIDebug() {
  bool enabled;

  enabled = (ctes_material.scalar_metallic >= 0.f);
  if (ImGui::Checkbox("Custom Metallic", &enabled)) {
    if (enabled)
      ctes_material.scalar_metallic = 0.f;
    else
      ctes_material.scalar_metallic = -1.f;
  }
  if (ctes_material.scalar_metallic >= 0.f)
    ImGui::DragFloat("Metallic", &ctes_material.scalar_metallic, 0.01f, 0.f, 1.f);

  enabled = (ctes_material.scalar_roughness >= 0.f);
  if (ImGui::Checkbox("Custom Roughness", &enabled)) {
    if (enabled)
      ctes_material.scalar_roughness = 0.f;
    else
      ctes_material.scalar_roughness = -1.f;
  }
  if(ctes_material.scalar_roughness >= 0.f ) 
    ImGui::DragFloat("Roughness", &ctes_material.scalar_roughness, 0.01f, 0.f, 1.f);

  ImGui::DragFloat("Irradiance vs mipmaps", &ctes_material.scalar_irradiance_vs_mipmaps, 0.01f, 0.f, 1.f);
}

void CMaterial::onStartElement(const std::string &elem, MKeyValue &atts) {
	if (elem == "material") {
		auto tech_name = atts["tech"];
		if (tech_name.empty())
			tech_name = "deferred.tech";
		tech = Resources.get(tech_name.c_str())->as<CRenderTechnique>();
    ctes_material.scalar_metallic = atts.getFloat("metallic", -1.f);
    ctes_material.scalar_roughness = atts.getFloat("roughness", -1.f);
    ctes_material.scalar_irradiance_vs_mipmaps = 0;
    ctes_material.scalar_dummy = -1;
	}
	else if (elem == "texture") {
		auto type_name = atts["slot"];  // diffuse/specular/...
		auto text_name = atts["name"];

		TTextureSlot type_slot;
		if (type_name == "diffuse" || type_name == "albedo") {
			type_slot = TTextureSlot::DIFFUSE;
		}
    else if (type_name == "roughness") {
      type_slot = TTextureSlot::ROUGHNESS;
    }
    else if (type_name == "metallic") {
      type_slot = TTextureSlot::METALLIC;
    }
    else if (type_name == "specular") {
      type_slot = TTextureSlot::SPECULAR;
    }
    else if (type_name == "normalmap") {
      type_slot = TTextureSlot::NORMALMAP;
    }
    else if (type_name == "environment") {
      type_slot = TTextureSlot::ENVIRONMENT;
    }
    else if (type_name == "irradiance") {
      type_slot = TTextureSlot::IRRADIANCE;
    }
		else {
			fatal("Invalid texture slot type %s found at material definition\n", type_name.c_str());
			return;
		}

		auto new_texture = Resources.get(text_name.c_str())->as<CTexture>();
		textures[type_slot] = new_texture;
	}
}

// ----------------------------------------------
bool CMaterial::load(const char* filename) {
	std::string full_path = getDataPath() + std::string(filename);
	bool is_ok = xmlParseFile(full_path);
	assert(is_ok);
	return true;
}

// ----------------------------------------------
void CMaterial::activateTextures() const {
<<<<<<< HEAD
  textures[DIFFUSE]->activate(TEXTURE_SLOT_DIFFUSE);
  assert(textures[NORMALMAP]);
  textures[NORMALMAP]->activate(TEXTURE_SLOT_NORMALS);
=======
  if (textures[DIFFUSE])
    textures[DIFFUSE]->activate(TEXTURE_SLOT_DIFFUSE);
  if (textures[NORMALMAP])
    textures[NORMALMAP]->activate(TEXTURE_SLOT_NORMALS);
>>>>>>> d6b4e6803fa82f01d5c091b986f30dbebbb8b427
  if (textures[METALLIC])
    textures[METALLIC]->activate(TEXTURE_SLOT_METALLIC);
  if (textures[ROUGHNESS])
    textures[ROUGHNESS]->activate(TEXTURE_SLOT_ROUGHNESS);
  if(textures[SPECULAR])
    textures[SPECULAR]->activate(TEXTURE_SLOT_SPECULAR);
  if (textures[ENVIRONMENT])
    textures[ENVIRONMENT]->activate(TEXTURE_SLOT_ENVIRONMENT);
  if (textures[IRRADIANCE])
    textures[IRRADIANCE]->activate(TEXTURE_SLOT_IRRADIANCE);
  
  *(TCteMaterial*)&shader_ctes_material = ctes_material;
  shader_ctes_material.uploadToGPU();
}

// ----------------------------------------------
void CMaterial::deactivateTextures() {
	CTexture::deactivate(TEXTURE_SLOT_DIFFUSE);
}