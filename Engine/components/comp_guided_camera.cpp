#include "mcv_platform.h"
#include "comp_guided_camera.h"
#include "comp_camera.h"
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
  //angularVelocity = atts.getFloat("angularVelocity", 0);
  for (int i = 0; i < num_points; i++) {
    WPT_ATR_NAME(atrPos, i);
    positions[i] = atts.getPoint(atrPos);
	LPT_ATR_NAME(atrLook, i);
	targets[i] = atts.getPoint(atrLook);
  }

  default_dirs = atts.getBool("default_dirs", 0);
  return true;
};

//TODO: Modificar o sustituir (auxiliar de SmoothInluence)
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
	start(msg.speed);
	//TODO: first point = main_camera (and target/rotation)
  }
}

bool TCompGuidedCamera::followGuide(TCompTransform* camTransform, TCompCamera* cam) {
  if (curPoint >= positions.size()) {
    return false;
  }
  if (io->keys['Q'].becomesPressed()) {
	  CHandle me = CHandle(this).getOwner();
	  CEntity* eMe = me;
	  logic_manager->throwEvent(CLogicManagerModule::EVENT::OnCinematicSkipped, std::string(eMe->getName())), CHandle(this).getOwner();
	  return false;
  }

  VEC3 goTo = positions[curPoint];

  CHandle tX = tags_manager.getFirstHavingTag("player");
  CEntity * target_eX = tX;

  TCompTransform * targettransX = target_eX->get<TCompTransform>();

  VEC3 nextPoint = positions[min(curPoint + 1, positions.size())];

  VEC3 pos = camTransform->getPosition();
  if (simpleDist(pos, goTo) > 0.5f) {
	//Going to target
	std::vector<VEC3> posCmr = std::vector<VEC3>(4); //Catmull Rom positions
	for (int i = 0; i < posCmr.size(); i++) {
		posCmr[i] = positions[min(curPoint - 2 + i, positions.size())];
	}

	//Factor = distancia recorrida este frame / distancia total punto actual y siguiente
    factor += (getDeltaTime() * velocity) / (realDist(goTo, nextPoint));
    pos = VEC3::CatmullRom(posCmr[0], posCmr[1], posCmr[2], posCmr[3], factor);
    camTransform->setPosition(pos);
  }
  else {
	//New target
    ++curPoint;
    factor = 0.0f;
  }

  if (default_dirs) {
    cam->smoothLookAt(camTransform->getPosition(), nextPoint, cam->getUpAux(), 0.5f);
  }
  else { //if (curPoint > 0) {
    VEC3 campos = camTransform->getPosition();
	
	//TODO: fix this method! (Influencia??)
    cam->smoothUpdateInfluence(camTransform, this, influencia, cam->getUpAux());	//smooth movement
  }
  return true;
}