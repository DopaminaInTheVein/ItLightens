#include "mcv_platform.h"
#include "skeleton_manager.h"

// ---------------- Utils ----------------------------------
CalVector Engine2Cal(VEC3 v) {
	return CalVector(v.x, v.y, v.z);
}
CalQuaternion Engine2Cal(CQuaternion q) {
	return CalQuaternion(q.x, q.y, q.z, -q.w);
}
VEC3 Cal2Engine(CalVector v) {
	return VEC3(v.x, v.y, v.z);
}
CQuaternion Cal2Engine(CalQuaternion q) {
	return CQuaternion(q.x, q.y, q.z, -q.w);
}
//--------------------------------------------------

CalQuaternion getRotationFromAToB(CalVector a, CalVector b, float unit_amount, float max_angle) {
	VEC3 da = Cal2Engine(a);
	VEC3 db = Cal2Engine(b);
	VEC3 normal = da.Cross(db);
	if (!isNormal(normal)) return Engine2Cal(CQuaternion::Identity);

	float angle = clampAbs(angleBetween(da, db), max_angle);
	CQuaternion q = CQuaternion::CreateFromAxisAngle(normal, angle * unit_amount);
	return Engine2Cal(q);
}
// --------------------------------------------------

CCoreModel::CCoreModel() : CalCoreModel("unnamed") {}

void CCoreModel::TBoneCorrector::apply(CalModel* model, CalVector world_pos, float amount, float max_angle, bool keep_vertical) {
	CalBone* bone = model->getSkeleton()->getBone(bone_id);
	assert(bone);

	// My coords in world coords
	CalVector bone_abs_pos = bone->getTranslationAbsolute();

	// The direction in world coords
	CalVector dir_abs = world_pos - bone_abs_pos;
	if (keep_vertical) dir_abs.y = 0;

	// Convert it to local coords of my bone
	CalQuaternion abs_to_local_rot = bone->getRotationAbsolute();
	abs_to_local_rot.invert();

	// Convert the world dir to local dir
	CalVector dir_local_to_target = dir_abs;
	dir_local_to_target *= abs_to_local_rot;

	CalQuaternion correction_rot = getRotationFromAToB(local_dir, dir_local_to_target, amount, max_angle);

	// The current bone rotation wrt my parent bone
	CalQuaternion bone_local_rotation = bone->getRotation();

	// Apply the correction and set it to the bone
	correction_rot *= bone_local_rotation;
	bone->setRotation(correction_rot);

	// Compute new abs rotation and update abs of my children bones
	bone->calculateState();
}