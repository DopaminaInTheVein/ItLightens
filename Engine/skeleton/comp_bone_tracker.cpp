#include "mcv_platform.h"
#include "comp_bone_tracker.h"
#include "components/entity_tags.h"
#include "components/entity.h"
#include "components/comp_transform.h"
#include "components/comp_physics.h"
#include "skeleton/comp_skeleton.h"
#include "skeleton/comp_skeleton_ik.h"
#include "cal3d/cal3d.h"

extern VEC3 Cal2Engine(CalVector v);
extern CQuaternion Cal2Engine(CalQuaternion q);

bool TCompBoneTracker::load(MKeyValue& atts) {
	strcpy(bone_name, atts.getString("bone", "").c_str());
	strcpy(entity_name, atts.getString("entity", "").c_str());
	bone_id = -1;
	h_entity = CHandle();
	return true;
}

void TCompBoneTracker::onGroupCreated(const TMsgEntityGroupCreated& msg) {
	h_entity = findByName(*msg.handles, entity_name);
	if (!h_entity.isValid()) return;
	GET_COMP(skel, h_entity, TCompSkeleton);
	if (!skel) return;
	bone_id = skel->model->getSkeleton()->getCoreSkeleton()->getCoreBoneId(bone_name);
}

void TCompBoneTracker::onAttach(const TMsgAttach& msg) {
	h_entity = msg.handle;
	if (!h_entity.isValid()) return;

	GET_COMP(skel, h_entity, TCompSkeleton);
	if (!skel) return;

	strcpy(bone_name, msg.bone_name.c_str());
	bone_id = skel->getKeyBoneId(bone_name);
	local_tmx_saved = msg.save_local_tmx;

	auto bone = skel->model->getSkeleton()->getBone(bone_id);
	VEC3 pos_bone_abs = Cal2Engine(bone->getTranslationAbsolute());
	CQuaternion rot_bone_abs = Cal2Engine(bone->getRotationAbsolute());
	dbg("Descompose bone abs. (%f, %f, %f), (%f, %f, %f, %f)\n"
		, VEC3_VALUES(pos_bone_abs)
		, VEC4_VALUES(rot_bone_abs));

	// BoneWorld inv
	MAT44 bone_world = MAT44::CreateFromQuaternion(rot_bone_abs);
	bone_world.Translation(pos_bone_abs);
	VEC3 scale, pos; CQuaternion rot;
	bone_world.Decompose(scale, rot, pos);
	dbg("Descompose bone_world. (%f, %f, %f), (%f, %f, %f, %f)\n"
		, VEC3_VALUES(pos)
		, VEC4_VALUES(rot));
	MAT44 bone_world_inv = bone_world.Invert();
	bone_world_inv.Decompose(scale, rot, pos);
	dbg("Descompose bone_world invert. (%f, %f, %f), (%f, %f, %f, %f)\n"
		, VEC3_VALUES(pos)
		, VEC4_VALUES(rot));

	// My World now
	GET_MY(tmx, TCompTransform);
	if (!tmx) return;

	MAT44 my_world;
	//Si no utiliza la transform actual, calculamos la inicial
	if (msg.save_local_tmx) {
		my_world = tmx->asMatrix();
	}
	else {
		my_world = MAT44::Identity;
		my_world.Translation(msg.offset);
		my_world *= bone_world;

		//tmx->setPosition(pos_bone_abs);
		//GET_COMP(phys, msg.handle, TCompPhysics);
		//if (phys) phys->setPosition(tmx);
	}

	my_world.Decompose(scale, rot, pos);
	dbg("Descompose my_world. (%f, %f, %f), (%f, %f, %f, %f)\n"
		, VEC3_VALUES(pos)
		, VEC4_VALUES(rot));
	// Local bone transform
	local_tmx = my_world * bone_world_inv;
	local_tmx.Decompose(scale, rot, pos);
	dbg("Descompose local_tmx. (%f, %f, %f), (%f, %f, %f, %f)\n"
		, VEC3_VALUES(pos)
		, VEC4_VALUES(rot));
	//}
}

void TCompBoneTracker::onAttachWithOffset(const TMsgAttach& msg) {
	h_entity = msg.handle;
	if (!h_entity.isValid()) return;
	GET_COMP(skel, h_entity, TCompSkeleton);
	if (!skel) return;

	strcpy(bone_name, msg.bone_name.c_str());
	bone_id = skel->getKeyBoneId(bone_name);
	local_tmx_saved = msg.save_local_tmx;
	auto bone = skel->model->getSkeleton()->getBone(bone_id);
	VEC3 pos_bone_abs = Cal2Engine(bone->getTranslationAbsolute());
	CQuaternion rot_bone_abs = Cal2Engine(bone->getRotationAbsolute());
	dbg("Descompose bone abs. (%f, %f, %f), (%f, %f, %f, %f)\n"
		, VEC3_VALUES(pos_bone_abs)
		, VEC4_VALUES(rot_bone_abs));

	// BoneWorld inv
	MAT44 bone_world = MAT44::CreateFromQuaternion(rot_bone_abs);
	bone_world.Translation(pos_bone_abs);
	VEC3 scale, pos; CQuaternion rot;
	bone_world.Decompose(scale, rot, pos);
	dbg("Descompose bone_world. (%f, %f, %f), (%f, %f, %f, %f)\n"
		, VEC3_VALUES(pos)
		, VEC4_VALUES(rot));
	MAT44 bone_world_inv = bone_world.Invert();
	bone_world_inv.Decompose(scale, rot, pos);
	dbg("Descompose bone_world invert. (%f, %f, %f), (%f, %f, %f, %f)\n"
		, VEC3_VALUES(pos)
		, VEC4_VALUES(rot));

	// My World now
	MAT44 my_world;
	if (msg.save_local_tmx) {
		GET_MY(tmx, TCompTransform); tmx->addPosition(msg.offset);
		MAT44 my_world = tmx->asMatrix();
	}
	else {
		my_world = MAT44::CreateFromQuaternion(rot_bone_abs);
		my_world.Translation(pos_bone_abs + msg.offset);
	}

	my_world.Decompose(scale, rot, pos);
	dbg("Descompose my_world. (%f, %f, %f), (%f, %f, %f, %f)\n"
		, VEC3_VALUES(pos)
		, VEC4_VALUES(rot));
	// Local bone transform
	local_tmx = my_world * bone_world_inv;
	local_tmx.Decompose(scale, rot, pos);
	dbg("Descompose local_tmx. (%f, %f, %f), (%f, %f, %f, %f)\n"
		, VEC3_VALUES(pos)
		, VEC4_VALUES(rot));
}

void TCompBoneTracker::renderInMenu() {
	bool changed = false;
	changed |= ImGui::InputText("Bone Name", bone_name, sizeof(bone_name));
	if (bone_id != -1)
		ImGui::Text("Bone %d", bone_id);
	h_entity.renderInMenu();
}

void TCompBoneTracker::update(float dt) {
	if (!h_entity.isValid()) return;

	GET_COMP(skel, h_entity, TCompSkeleton);
	if (!skel || bone_id == -1) return;

	auto bone = skel->model->getSkeleton()->getBone(bone_id);
	auto rot = Cal2Engine(bone->getRotationAbsolute());
	auto trans = Cal2Engine(bone->getTranslationAbsolute());

	GET_MY(tmx, TCompTransform);
	//assert(tmx);
	if (!tmx) return;

	//if (local_tmx_saved) {
	MAT44 bone_world = MAT44::CreateFromQuaternion(rot);
	bone_world.Translation(trans);
	MAT44 new_tmx = local_tmx * bone_world;
	VEC3 scale;
	new_tmx.Decompose(scale, rot, trans);
	//}

	tmx->setPosition(trans);
	tmx->setRotation(rot);

	GET_COMP(physics, CHandle(this).getOwner(), TCompPhysics);
	if (physics) {
		physics->setPosition(trans, rot);
	}
}