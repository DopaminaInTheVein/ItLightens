#ifndef INC_COMPONENT_TEXT_H_
#define INC_COMPONENT_TEXT_H_

#include "comp_base.h"

class TCompText : public TCompBase {
	std::string id;
	VHandles ui_letters;
	float scale = 0.0f;
	float marginForImage;
	float letter_posx_ini;
	float letter_posy_ini;
	VEC4 color;
	int lines;
	float letterBoxSize = 1.0f / 16.0f;

	std::string text;
	std::vector<std::string> lineText;
	float  ttl;
	float sizeFontX = 0.0125f;
	float sizeFontY = 0.030f;
	bool printed = false;
	std::vector<float> accumSpacing;

public:
	//Update info
	void update(float elapsed);
	void printLetters();
	void uploadCtes();
	void forceTTLZero();
	bool load(MKeyValue& atts);
	// Getter
	const std::string getId() const { return id; }
};

#endif
