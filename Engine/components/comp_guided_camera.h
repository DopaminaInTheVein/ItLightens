#ifndef INC_COMPONENT_GUIDED_CAMERA_H_
#define INC_COMPONENT_GUIDED_CAMERA_H_

#include "utils/XMLParser.h"
#include "comp_base.h"

class TCompGuidedCamera : public TCompBase {
	std::vector<VEC3> points;
	int num_points;
	int num_cameras;
	std::vector<CQuaternion> rotations;
	std::vector<float> influences;
	std::vector<VEC3> cameraPositions;

public:
	bool load(MKeyValue& atts);
	int nearCameraPoint(VEC3 playerPosition);
	CQuaternion getNewRotationForCamera(VEC3 playerPosition, CQuaternion cameraActual, int pointOfInfluence, float acumulatedTime);
}

#endif