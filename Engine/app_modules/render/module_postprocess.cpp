#include "mcv_platform.h"
#include "module_postprocess.h"
#include "components\comp_tags.h"
#include "components\entity.h"
#include "components\comp_camera_main.h"

//fx include
#include "postprocess\comp_render_fade_screen.h"

bool CPostProcessModule::start()
{
	CEntity* e_c = tags_manager.getFirstHavingTag("camera_main");
	if (e_c) {
		TCompCameraMain* cam = e_c->get<TCompCameraMain>();

		//list fx (order is improtant!!!):

		//hatching

		//outline

		//screen fade effect
		TFadeScreenFX *fx = e_c->get<TFadeScreenFX>();
		//fxs.push_back(fx);


	}
	else {
		fatal("MAIN CAMERA DONT EXIST");
		return false;
	}


	return true;
}

void CPostProcessModule::stop()
{
	fxs.clear();
}

void CPostProcessModule::update(float dt)
{
	for (auto& fx : fxs) {
		fx->update(dt);
	}
}

void CPostProcessModule::render()
{
	for (auto& fx : fxs) {
		fx->render();
	}
}
