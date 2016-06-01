#ifndef INC_RENDER_MATERIAL_H_
#define INC_RENDER_MATERIAL_H_

class CTexture;
class CRenderTechnique;

#include "constants/ctes_material.h"

class CMaterial : public IResource, public CXMLParser {
	void onStartElement(const std::string &elem, MKeyValue &atts) override;

public:

	enum TTextureSlot {
		  DIFFUSE = 0
		, SPECULAR
    , NORMALMAP
    , METALLIC
    , ROUGHNESS

    , IRRADIANCE
    , ENVIRONMENT
    , COUNT
	};
	const CRenderTechnique*   tech;
	const CTexture*           textures[COUNT];

  TCteMaterial              ctes_material;

	CMaterial()
		: tech(nullptr)
	{
		for (int i = 0; i < TTextureSlot::COUNT; ++i)
			textures[i] = nullptr;
	}

	bool isValid() const override { return true; }
	void destroy() { }
  eType getType() const override { return MATERIAL; }

	void renderUIDebug();
	bool load(const char* filename);

	void activateTextures() const;
	static void deactivateTextures();
};

#endif
