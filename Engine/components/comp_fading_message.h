#ifndef INC_COMPONENT_FADING_MESSAGE_H_
#define INC_COMPONENT_FADING_MESSAGE_H_

#include "comp_base.h"

class TCompFadingMessage : public TCompBase {
	float percentLineHeight = 0.02f;
	float marginForImage = 0.1f;
	float timeForLetter = 0.05f;
	int numchars;
	int lines;
	float resolution_x;
	float resolution_y;
	ImGuiWindowFlags flags;
	float startxrect;
	float startyrect;

	std::string text;
	ImColor textColor;
	ImColor backgroudColor;
	float  ttl;
	float sizeFont = 0.025f;

public:
	//Update info

	void update(float elapsed);
	void render() const;
	bool load(MKeyValue& atts);
};

#endif
