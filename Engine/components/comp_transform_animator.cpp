#include "mcv_platform.h"
#include "comp_transform.h"
#include "comp_transform_animator.h"
#include "components/entity.h"

using namespace std;

bool TCompTransformAnimator::load(MKeyValue & atts)
{
	yaw_speed = deg2rad(atts.getFloat("yaw_speed", 0.f));
	pitch_speed = deg2rad(atts.getFloat("pitch_speed", 0.f));
	roll_speed = deg2rad(atts.getFloat("roll_speed", 0.f));
	return true;
}

bool TCompTransformAnimator::getUpdateInfo()
{
	tmx = GETH_MY(TCompTransform);
	if (!tmx) return false;
	return true;
}

#define tmx_anim_inc(name) name += name##_speed * dt
void TCompTransformAnimator::update(float dt)
{
	float yaw, pitch, roll;
	tmx->getAngles(&yaw, &pitch, &roll);
	tmx_anim_inc(yaw);
	tmx_anim_inc(pitch);
	tmx_anim_inc(roll);
	tmx->setAngles(yaw, pitch, roll);
}