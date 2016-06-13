#include "mcv_platform.h"
#include "comp_guided_camera.h"
#include "comp_camera.h"
#include "comp_controller_3rd_person.h"
#include "comp_transform.h"
#include "entity.h"

#include "app_modules/logic_manager/logic_manager.h"

#define WPT_ATR_NAME(nameVariable, index) \
char nameVariable[10]; sprintf(nameVariable, "wpt%d", index)
#define LPT_ATR_NAME(nameVariable, index) \
char nameVariable[10]; sprintf(nameVariable, "lpt%d", index)

bool TCompGuidedCamera::load(MKeyValue& atts) {
	num_points = atts.getInt("points_size", 0);
	velocity_default = atts.getFloat("velocity", 0);

	//La posicion 0 es la actual en el momento de empezar la cinematica!
	//Y anadimos otro punto extra inventado al final para catmull rom
	positions.resize(num_points + 2);
	targets.resize(num_points + 2);
	for (int i = 1; i <= num_points; i++) {
		WPT_ATR_NAME(atrPos, i);
		positions[i] = atts.getPoint(atrPos);
		LPT_ATR_NAME(atrLook, i);
		targets[i] = atts.getPoint(atrLook);
	}

	positions[num_points + 1] = positions[num_points];
	targets[num_points + 1] = targets[num_points];

	default_dirs = atts.getBool("default_dirs", 0);
	return true;
};

void TCompGuidedCamera::start(float speed) {
	curPoint = 1;
	factor = 0.0f;
	velocity = speed == 0.f ? velocity_default : speed;
}

void TCompGuidedCamera::onGuidedCamera(const TMsgGuidedCamera& msg) {
	//Get main_camera and notify
	CHandle mainCamera = tags_manager.getFirstHavingTag("camera_main");
	if (mainCamera.isValid()) {
		//Messagge to sent
		TMsgGuidedCamera msgToMainCamera;
		msgToMainCamera.guide = CHandle(this).getOwner();

		//Send message
		CEntity* eMainCamera = mainCamera;
		eMainCamera->sendMsg(msgToMainCamera);

		//Init Guide
		TCompGuidedCamera::start(msg.speed);

		//Initial Pos
		TCompCameraMain * comp_main_camera = eMainCamera->get<TCompCameraMain>();
		assert(comp_main_camera);
		positions[0] = comp_main_camera->getPosition();

		//Initial Target
		TCompController3rdPerson * camera_controller = eMainCamera->get<TCompController3rdPerson>();
		assert(camera_controller);
		targets[0] = positions[0] + comp_main_camera->getFront() * camera_controller->GetPositionDistance();
	}
}

bool TCompGuidedCamera::followGuide(TCompTransform* camTransform, TCompCameraMain* cam) {
	if (curPoint > positions.size() - 2) {
		return false;
	}
	if (io->keys['Q'].becomesPressed()) {
		CHandle me = CHandle(this).getOwner();
		CEntity* eMe = me;
		logic_manager->throwEvent(CLogicManagerModule::EVENT::OnCinematicSkipped, std::string(eMe->getName())), CHandle(this).getOwner();
		return false;
	}

	VEC3 goTo = positions[curPoint];
	VEC3 comeFrom = positions[curPoint - 1];
	VEC3 lookTo = targets[curPoint];
	VEC3 lookFrom = targets[curPoint - 1];

	if (factor < 1.f) {
		//Going to target
		std::vector<VEC3> posCmr = std::vector<VEC3>(4); //Catmull Rom positions
		std::vector<VEC3> lookCmr = std::vector<VEC3>(4); //Catmull Rom positions
		for (int i = 0; i < posCmr.size(); i++) {
			int index = clamp(curPoint - 2 + i, 0, positions.size() - 1);
			posCmr[i] = positions[index];
			lookCmr[i] = targets[index];
		}
		//Factor = distancia recorrida este frame / distancia total punto actual y siguiente
		float moveAmount = getDeltaTime() * velocity;

		factor += smoothFactor * (getDeltaTime() * velocity) / (realDist(comeFrom, goTo));
		VEC3 pos = VEC3::CatmullRom(posCmr[0], posCmr[1], posCmr[2], posCmr[3], factor);
		VEC3 look = default_dirs
			? positions[clamp(curPoint + 1, 0, positions.size() - 1)]
			: VEC3::CatmullRom(lookCmr[0], lookCmr[1], lookCmr[2], lookCmr[3], factor);
		cam->smoothLookAt(pos, look, cam->getUpAux(), smoothFactor / getDeltaTime());
		camTransform->lookAt(pos, look, cam->getUpAux());
		Debug->DrawLine(comeFrom, goTo);
		Debug->DrawLine(lookFrom, lookTo, VEC3(0, 0, 1));
	}
	else {
		//New target
		dbg("End point(%d), factor = %f.3\n", curPoint, factor);
		++curPoint;
		factor = 0.0f;
	}
	return true;
}