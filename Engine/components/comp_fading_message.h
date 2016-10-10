#ifndef INC_COMPONENT_FADING_MESSAGE_H_
#define INC_COMPONENT_FADING_MESSAGE_H_

#include "comp_base.h"

class TCompFadingMessage : public TCompBase {
	VHandles gui_letters;
	CHandle gui_back;
	CHandle gui_sci;
	CHandle gui_mol;
	CHandle gui_rai;

	//int id;
	bool permanent;
	float percentLineHeight = 0.02f;
	float timeForLetter = 0.05f;
	float marginForImage;
	int numchars;
	int shown_chars;
	int lines;
	float scale = 0.2f;
	float letterBoxSize = 1.0f / 16.0f;
	float letterSpacer = 0.3f / 16.0f;
	float letterSpacerHigh = 2.5f * 0.3f / 16.0f;

	std::string text;
	std::vector<std::string> lineText;
	float  ttl;
	float sizeFontX = 0.0125f;
	float sizeFontY = 0.030f;
	std::vector<float> accumSpacing;
	bool enabled = false;
	bool initialized = false;
	float accumTime = 0.0f;
	VEC3 min_ortho;
	VEC3 max_ortho;
	VEC3 orthorect;
	void moveElement(CHandle h, const VEC3 pos);
public:
	//Update info
	void update(float elapsed);
	void printLetters();
	void Init();
	bool load(MKeyValue& atts);
	void hideAll();
	void kill() { ttl = -1.f; }
};

#endif
