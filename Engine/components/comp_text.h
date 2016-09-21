#ifndef INC_COMPONENT_TEXT_H_
#define INC_COMPONENT_TEXT_H_

#include "comp_base.h"

class TCompText : public TCompBase {
	VHandles gui_letters;
	std::string id = "";
	float scale = 0.0f;
	float marginForImage;
	float letter_posx_ini;
	float letter_posy_ini;
	float letter_posz_ini;
	VEC4 color;
	VEC4 colorTarget;
	float colorChangeSpeed;
	float colorChangeSpeedLag;
	int lines;
	float letterBoxSize = 1.0f / 16.0f;
	bool loop = false;

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
	void forceTTLZero();
	bool load(MKeyValue& atts);
	// Getter
	const std::string getId() const { return id; }
	// Setter
	void setup(std::string id, std::string text, float posx, float posy, std::string textColor, float scale, std::string set_textColorTarget, float set_textColorSpeed, float set_textColorSpeedLag, float set_posz = 0.5f, bool loop = true);
	void SetText(std::string text);
	void SetPosWorld(VEC3 pos);
	void SetPosScreen(VEC3 pos);
	void SetSize(float);
	void SetColor(std::string rrggbbaa);
	void SetColorTarget(std::string rrggbbaa, float speed = 1.f);
	void SetLetterLag(float);
	void setAttr(float new_x, float new_y, float new_scale);
	void Move(VEC3 pos);
	void SetZ(float z);
	TCompText::~TCompText();
};

#endif
