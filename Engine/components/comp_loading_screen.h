#ifndef INC_COMPONENT_LOADING_SCREEN_H_
#define INC_COMPONENT_LOADING_SCREEN_H_

#include "comp_base.h"

class TCompLoadingScreen : public TCompBase {
	float resolution_x;
	float resolution_y;
	int loading_value = 0;
	int decenas = 0;
	int unidades = 0;
	ImGuiWindowFlags flags;
	CHandle letras[100];

	float letterBoxSize = 1.0f;
	int shown_chars;
	int numchars;
	int lines = 1;

	int id = std::rand();
	std::string text;
	std::vector<std::string> lineText;
	float  ttl = 0.5f;
	float sizeFontX = 0.0125f;
	float sizeFontY = 0.030f;
	float timeForLetter = 0.05f;

public:
	//Update info
	void update(float elapsed);
	void render() const;
	bool load(MKeyValue& atts);
	//void printLetters() const;
	//void updateLetters(bool loaded) const;
	void renderInMenu();
	void onCreate(const TMsgEntityCreated&);
};

#endif
