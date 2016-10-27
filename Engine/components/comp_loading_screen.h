#ifndef INC_COMPONENT_LOADING_SCREEN_H_
#define INC_COMPONENT_LOADING_SCREEN_H_

#include "comp_base.h"

class TCompLoadingScreen : public TCompBase {
	int loading_value = 0;
	CHandle h_loading_bar;
	CHandle h_pag1;
	CHandle h_pag2;
	void updateHandle(CHandle &h, std::string tag);
	int cur_page = 0;
	float time_page = 0.f;
	float max_time_page = 2.f;
	void tooglePages();
	void swapPages();
public:
	//Update info
	void update(float elapsed);
	bool load(MKeyValue& atts);
	//void printLetters() const;
	//void updateLetters(bool loaded) const;
	void renderInMenu();
	void onCreate();
};

#endif
