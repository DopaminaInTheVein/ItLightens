#include "mcv_platform.h"
#include "resources/resources_manager.h"
#include "material.h"
#include "texture.h"
#include "constants/ctes_platform.h"

template<> IResource::eType getTypeOfResource<CMaterial>() { return IResource::MATERIAL; }

template<>
IResource* createObjFromName<CMaterial>(const std::string& name) {
	CMaterial* obj = new CMaterial;
	if (!obj->load(name.c_str())) {
		dbg("Can't load CMaterial %s. Will try placeholder...\n", name.c_str());
		// try to load a placeholder...
		//if (!texture->load("missing.dds")) {
		//  fatal("Can't load texture %s\n", name.c_str());
		//}
	}
	obj->setName(name.c_str());
	return obj;
}

void CMaterial::onStartElement(const std::string &elem, MKeyValue &atts) {
	if (elem == "material") {
		auto tech_name = atts["tech"];
		if (tech_name.empty())
			tech_name = "deferred.tech";
		tech = Resources.get(tech_name.c_str())->as<CRenderTechnique>();
	}
	else if (elem == "texture") {
		auto type_name = atts["slot"];  // diffuse/specular/...
		if (type_name == "") type_name = atts["type"];
		assert(type_name != "");
		auto text_name = atts["name"];

		TTextureSlot type_slot;
		if (type_name == "diffuse") {
			type_slot = TTextureSlot::DIFFUSE;
		}
		//else if (type_name == "bump") {
			//type_slot = TTextureSlot::BUMP;
		//}
		else if (type_name == "specular") {
			type_slot = TTextureSlot::SPECULAR;
		}
	  	else if (type_name == "normalmap") {
	      		type_slot = TTextureSlot::NORMALMAP;
	    	}
	    	else if (type_name == "environment") {
	      		type_slot = TTextureSlot::ENVIRONMENT;
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
	PROFILE_FUNCTION("render material: activate textures");
	/*
	for (int i = 0; i < TTextureSlot::COUNT; i++) {
		if (textures[i]) {
			if (textures[i] && i > 0) {
				int i = 0;
			}
			textures[i]->activate(i);
		}
	}*/

	textures[DIFFUSE]->activate(TEXTURE_SLOT_DIFFUSE);
  	//assert(textures[NORMALMAP]);
	if(textures[NORMALMAP])
  		textures[NORMALMAP]->activate(TEXTURE_SLOT_NORMALS);
  	if( textures[ENVIRONMENT] )
    		textures[ENVIRONMENT]->activate(TEXTURE_SLOT_ENVIRONMENT);
}

// ----------------------------------------------
void CMaterial::deactivateTextures() {
	CTexture::deactivate(TEXTURE_SLOT_DIFFUSE);
}