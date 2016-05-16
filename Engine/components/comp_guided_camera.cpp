#include "mcv_platform.h"
#include "comp_guided_camera.h"
#include "comp_camera.h"
#include "comp_transform.h"
#include "entity.h"

#include "app_modules/logic_manager/logic_manager.h"

#define WPT_ATR_NAME(nameVariable, index) \
char nameVariable[10]; sprintf(nameVariable, "wpt%d", index)
#define ROT_ATR_NAME(nameVariable, index) \
char nameVariable[10]; sprintf(nameVariable, "rot%d", index)

bool TCompGuidedCamera::load(MKeyValue& atts) {
  num_points = atts.getInt("points_size", 0);
  velocity_default = atts.getFloat("velocity", 0);
  //angularVelocity = atts.getFloat("angularVelocity", 0);
  num_cameras = num_points - 1;
  points.resize(num_points);
  rotations.resize(num_cameras);
  influences.resize(num_cameras);
  cameraPositions.resize(num_cameras);
  for (int i = 0; i < num_points; i++) {
    WPT_ATR_NAME(atrPos, i);
    points[i] = atts.getPoint(atrPos);
  }
  for (int i = 0; i < num_cameras; i++) {
    ROT_ATR_NAME(atrRot, i);
    CQuaternion rot = atts.getQuat(atrRot);
    rot.Normalize();
    rotations[i] = rot;
    influences[i] = realDist(points[i], points[i + 1]);
    VEC3 vectorBetween = (points[i + 1] - points[i]);
    vectorBetween.x /= 2;
    vectorBetween.y /= 2;
    vectorBetween.z /= 2;
    cameraPositions[i] = points[i] + vectorBetween;
  }
  default_dirs = atts.getBool("default_dirs", 0);

  return true;
};
/*
int TCompGuidedCamera::nearCameraPoint(VEC3 playerPosition) {
  int pos = -1;
  float latestInfluence = 999.9f;
  for (int i = 0; i < num_cameras; ++i) {
    float dist = realDist(playerPosition, cameraPositions[i]);
    if (dist < influences[i] && dist < latestInfluence) {
      latestInfluence = dist;
      pos = i;
    }
  }
  return pos;
};
*/
CQuaternion TCompGuidedCamera::getNewRotationForCamera(VEC3 playerPosition, CQuaternion cameraActual, int pointOfInfluence) {
  if (pointOfInfluence != lastP) {
    lastP = pointOfInfluence;
    maxInfluence = 0.0f;
    if (pointOfInfluence > 0) {
      last_quat = rotations[pointOfInfluence - 1];
    }
  }
  cameraActual.Normalize();

  if (pointOfInfluence < 0 || pointOfInfluence >= num_cameras) {
    return cameraActual;
  }

  float dist = realDist(playerPosition, points[pointOfInfluence + 1]);
  float distanciaUnitaria = dist / influences[pointOfInfluence];
  distanciaUnitaria = (1 - distanciaUnitaria);
  distanciaUnitaria = fmaxf(maxInfluence, distanciaUnitaria);
  maxInfluence = distanciaUnitaria;
  CQuaternion cameraNova;
  if (pointOfInfluence > 0) {
    cameraNova = CQuaternion::Slerp(last_quat, rotations[pointOfInfluence], distanciaUnitaria);
  }
  else {
    cameraNova = CQuaternion::Slerp(cameraActual, rotations[pointOfInfluence], distanciaUnitaria);
  }
  cameraNova.Normalize();
  return cameraNova;
};

void TCompGuidedCamera::start(float speed) {
	lastguidedCamPoint = 0;
	factor = 0.0f;
	velocity = speed == 0.f ? velocity_default : speed;
	
}

void TCompGuidedCamera::onGuidedCamera(const TMsgGuidedCamera& msg) {
	//Init Guide
	start(msg.speed);

	//Messagge to sent
	TMsgGuidedCamera msgToMainCamera;
	msgToMainCamera.guide = CHandle(this).getOwner();

	//Get main_camera and notify
	CHandle mainCamera = tags_manager.getFirstHavingTag("camera_main");
	if (mainCamera.isValid()) {
		CEntity* eMainCamera = mainCamera;
		eMainCamera->sendMsg(msgToMainCamera);
	}
}

bool TCompGuidedCamera::followGuide(TCompTransform* camTransform, TCompCamera* cam) {
	VEC3 goTo = getPointPosition(lastguidedCamPoint);

	CHandle tX = tags_manager.getFirstHavingTag("player");
	CEntity * target_eX = tX;

	TCompTransform * targettransX = target_eX->get<TCompTransform>();

	VEC3 nextPoint = (lastguidedCamPoint == getTotalPoints() - 1 ? getPointPosition(lastguidedCamPoint) : getPointPosition((lastguidedCamPoint + 1) % getTotalPoints()));
	/*
	float yaw, pitch;
	tmx->getAngles(&yaw, &pitch);

	if (!tmx->isHalfConeVision(goTo, deg2rad(1.0f))) {
	  float delta_yaw = tmx->getDeltaYawToAimTo(goTo);
	  if (abs(delta_yaw) > 0.001f) {
		yaw += delta_yaw*getDeltaTime()*gc->getAngularVelocity();
	  }
	  else {
		yaw += delta_yaw*gc->getAngularVelocity();
	  }
	}
	if (!tmx->isHalfConeVisionPitch(goTo, deg2rad(1.0f))) {
	  float delta_pitch = tmx->getDeltaPitchToAimTo(goTo);
	  if (abs(delta_pitch) > 0.001f) {
		pitch += delta_pitch*gc->getAngularVelocity()*getDeltaTime();
	  }
	  else {
		pitch += delta_pitch*gc->getAngularVelocity();
	  }
	}
	tmx->setAngles(yaw, pitch);
	*/
	VEC3 pos = camTransform->getPosition();
	if (simpleDist(pos, goTo) > 0.5f) {
		VEC3 fro = goTo - pos;
		fro.Normalize();

		if (lastguidedCamPoint < 2 || lastguidedCamPoint + 1 == getTotalPoints()) {
			pos = pos + (fro * getVelocity() * getDeltaTime());
		}
		else {
			VEC3 pos1 = getPointPosition(lastguidedCamPoint - 2), 
				pos2 = getPointPosition(lastguidedCamPoint - 1), 
				pos3 = goTo, 
				pos4 = getPointPosition(lastguidedCamPoint + 1);
			float veloc = getVelocity() / realDist(pos3, pos2);

			factor += getDeltaTime() * veloc;
			VEC3 posNew = VEC3::CatmullRom(pos1, pos2, pos3, pos4, factor);
			pos = posNew;
		}
		camTransform->setPosition(pos);
	}
	else {
		++lastguidedCamPoint;
		factor = 0.0f;
	}

	if (lastguidedCamPoint >= getTotalPoints() || io->keys['Q'].becomesPressed()) {
		return false;
	}
	else if (getDefaultDirsEnabled()) {
		cam->smoothLookAt(camTransform->getPosition(), nextPoint, cam->getUpAux(), 0.5f);
	}
	else if (lastguidedCamPoint > 0) {
		VEC3 campos = camTransform->getPosition();
		int influencia = lastguidedCamPoint - 1; // gc->nearCameraPoint(campos);
		cam->smoothUpdateInfluence(camTransform, this, influencia, cam->getUpAux());	//smooth movement
	}
	else {
		cam->smoothLookAt(camTransform->getPosition(), 
			camTransform->getPosition() + camTransform->getFront(), 
			cam->getUpAux());
	}
	return true;
}