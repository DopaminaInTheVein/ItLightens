#ifndef INC_COMPONENT_FADING_MESSAGE_H_
#define INC_COMPONENT_FADING_MESSAGE_H_

#include "comp_base.h"

class TCompFadingMessage : public TCompBase {
	float percentLineHeight = 0.02f;
	float timeForLetter = 0.05f;
	float marginForImage;
	int numchars;
	int shown_chars;
	int lines;
	float letterBoxSize = 1.0f / 16.0f;

	std::string text;
	std::vector<std::string> lineText;
	float  ttl;
	float sizeFontX = 0.025f;
	float sizeFontY = 0.060f;

public:
	//Update info
	void update(float elapsed);
	void render() const;
	void forceTTLZero();
	bool load(MKeyValue& atts);
};

#endif
