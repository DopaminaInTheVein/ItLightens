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

  positions[num_points + 1] = positions[num_points - 1] +
	  (positions[num_points] - positions[num_points - 1]) * 2;
  default_dirs = atts.getBool("default_dirs", 0);
  return true;
};

/*
VEC3 TCompGuidedCamera::getNewTargetForCamera(VEC3 playerPosition, VEC3 cameraActual, int pointOfInfluence) {
  if (pointOfInfluence != lastP) {
    lastP = pointOfInfluence;
    maxInfluence = 0.0f;
    if (pointOfInfluence > 0) {
      last_quat = rotations[pointOfInfluence - 1];
    }
  }
 // cameraActual.Normalize();

  if (pointOfInfluence < 0 || pointOfInfluence >= num_cameras) {
    return cameraActual;
  }

  float dist = realDist(playerPosition, points[pointOfInfluence + 1]);
  float distanciaUnitaria = dist / influences[pointOfInfluence];
  distanciaUnitaria = (1 - distanciaUnitaria);
  distanciaUnitaria = fmaxf(maxInfluence, distanciaUnitaria);
  maxInfluence = distanciaUnitaria;
  VEC3 cameraNova;
  if (pointOfInfluence > 0) {
    cameraNova = VEC3::Lerp(last_quat, rotations[pointOfInfluence], distanciaUnitaria);
  }
  else {
    cameraNova = VEC3::Lerp(cameraActual, rotations[pointOfInfluence], distanciaUnitaria);
  }
  //cameraNova.Normalize();
  return cameraNova;
};
*/
void TCompGuidedCamera::start(float speed) {
  curPoint = 0;
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
	TCompCamera * comp_main_camera = eMainCamera->get<TCompCamera>();
	assert(comp_main_camera);
	positions[0] = comp_main_camera->getPosition();

	//Initial Target
	TCompController3rdPerson * camera_controller = eMainCamera->get<TCompController3rdPerson>();
	assert(camera_controller);
	targets[0] = positions[0] + comp_main_camera->getFront() * camera_controller->GetPositionDistance();
  }
}

bool TCompGuidedCamera::followGuide(TCompTransform* camTransform, TCompCamera* cam) {
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
  VEC3 nextPoint = positions[clamp(curPoint + 1, 0, positions.size()-1)];

  VEC3 pos = camTransform->getPosition();
  assert(isValid(pos));
  VEC3 look = nextPoint;
  float dist = simpleDist(pos, goTo);
  dbg("dist: %f\n", dist);
  if (dist > 1.f) {
	//Going to target
	std::vector<VEC3> posCmr = std::vector<VEC3>(4); //Catmull Rom positions
	std::vector<VEC3> lookCmr = std::vector<VEC3>(4); //Catmull Rom positions
	for (int i = 0; i < posCmr.size(); i++) {
		posCmr[i] = positions[clamp(curPoint - 2 + i, 0, positions.size()-1)];
		lookCmr[i] = targets[clamp(curPoint - 2 + i, 0, positions.size()-1)];
	}
	//Factor = distancia recorrida este frame / distancia total punto actual y siguiente
	float moveAmount = getDeltaTime() * velocity;

	//TODO: factor para look distinto!
	factor += (getDeltaTime() * velocity) / (realDist(goTo, nextPoint));
	pos = VEC3::CatmullRom(posCmr[0], posCmr[1], posCmr[2], posCmr[3], factor);
	assert(isValid(pos));
	look = default_dirs 
		? nextPoint 
		: VEC3::CatmullRom(lookCmr[0], lookCmr[1], lookCmr[2], lookCmr[3], factor);
	cam->smoothLookAt(pos, look, cam->getUpAux(), 0.5f);
	camTransform->lookAt(pos, look, cam->getUpAux());
    Debug->DrawLine(pos, look);
	dbg("Pos: %.3f, %.3f, %.3f. Look: %.3f %.3f %.3f\n",
		pos.x, pos.y, pos.z,
		look.x, look.y, look.z);
  }
  else {
	//New target
    ++curPoint;
    factor = 0.0f;
	dbg("============================\n");
	dbg("Current Point = %d\n", curPoint);
	dbg("============================\n");
  }
  return true;
}