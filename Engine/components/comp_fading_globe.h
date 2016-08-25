#ifndef INC_COMPONENT_FADING_GLOBE_H_
#define INC_COMPONENT_FADING_GLOBE_H_

#include "comp_base.h"

class TCompFadingGlobe : public TCompBase {
	float percentLineHeight = 0.02f;
	float timeForLetter = 0.05f;
	float marginForImage;
	int numchars;
	int lines;
	int minlines = 3;
	float resolution_x;
	float resolution_y;
	ImGuiWindowFlags flags;
	float globe_width;
	float globe_height;
	// aux params for computing the correct position and size
	float distance;
	float char_x;
	float char_y;
	float char_z;
	// position of the globe
	float screen_x;
	float screen_y;

	std::string text;
	ImColor textColor;
	ImColor backgroudColor;
	float  ttl;
	float sizeFont = 0.025f;
	const CTexture * textureIcon;
	ImTextureID tex_id;

public:
	//Update info

	void update(float elapsed);
	void render() const;
	bool load(MKeyValue& atts);
};

#endif
