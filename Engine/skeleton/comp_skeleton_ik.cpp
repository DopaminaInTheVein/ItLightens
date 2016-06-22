#include "mcv_platform.h"
#include "comp_skeleton_ik.h"
#include "comp_skeleton.h"
#include "handle/handle.h"
#include "components/entity.h"
#include "skeleton/ik_handler.h"
#include "components/comp_transform.h"
#include "cal3d/cal3d.h"
#include "skeleton/skeleton_manager.h"

bool TCompSkeletonIK::load(MKeyValue &atts) {
  //target = atts.getPoint("target");
  //amount = atts.getFloat("amount", 1.0f);
  mods[0].bone_id = atts.getInt("bone_c0", -1);
  mods[1].bone_id = atts.getInt("bone_c1", -1);
  mods[0].normal = atts.getPoint("normal_c0");
  mods[1].normal = atts.getPoint("normal_c1");
  return true;
}

void TCompSkeletonIK::solveBone(TBoneMod* bm) {
  int bone_id_c = bm->bone_id;

  // Access to the sibling comp skeleton component
  // where we can access the cal_model instance
  CEntity* e = CHandle(this).getOwner();
  TCompSkeleton *comp_skel = e->get<TCompSkeleton>();
  if (comp_skel == nullptr)
    return;

  // Get end of IK Bone
  CalModel* model = comp_skel->model;
  assert(bone_id_c != -1);
  CalBone* bone_c = model->getSkeleton()->getBone(bone_id_c);

  // Bone 'B' is the middle bone
  int bone_id_b = bone_c->getCoreBone()->getParentId();
  assert(bone_id_b != -1);
  CalBone* bone_b = model->getSkeleton()->getBone(bone_id_b);

  // Bone 'A' is the up bone
  int bone_id_a = bone_b->getCoreBone()->getParentId();
  assert(bone_id_a != -1);
  CalBone* bone_a = model->getSkeleton()->getBone(bone_id_a);

  // Get Absolute positions of the bone based on post Cal3d update
  TIKHandle ik;
  ik.A = Cal2Engine(bone_a->getTranslationAbsolute());
  ik.B = Cal2Engine(bone_b->getTranslationAbsolute());
  ik.C = Cal2Engine(bone_c->getTranslationAbsolute());

  // Distance from a to b, based on the skel CORE definition
  CalVector cal_ab = bone_b->getCoreBone()->getTranslationAbsolute()
  - bone_a->getCoreBone()->getTranslationAbsolute();
  ik.AB = cal_ab.length();
  VEC3 ab2 = XMVector3Length(ik.A - ik.B);

  // Distance from b to c
  CalVector cal_bc = bone_c->getCoreBone()->getTranslationAbsolute()
  - bone_b->getCoreBone()->getTranslationAbsolute();
  ik.BC = cal_bc.length();
  VEC3 bc2 = XMVector3Length(ik.B - ik.C);

  // Lo anterior tendria que cachearse o inicializarse solo una vez
  // ...

  // Algo para calcula la altura al suelo...
  //float distance_of_c_to_ground = ground.distanceToGround(ik.C);
  //TODO: OJO CLH TODO_IK
  float distance_of_c_to_ground = 1.f;

  // Dado que el pie antes de hacer las modificaciones no esta sobre el plano
  // exactamente sino que los artistas lo han subido un poco obtener esa 
  // distancia...
  TCompTransform* tmx = e->get< TCompTransform >();
  float height_of_e = XMVectorGetY(tmx->getPosition());
  float height_of_c = XMVectorGetY(ik.C);

  float offset_of_over_ground = height_of_c - height_of_e;
  distance_of_c_to_ground -= offset_of_over_ground;

  // ------------------------------------------------
  // Correct the foot orientation
  // Obtain the current abs front and left directions in local coords
  CalVector abs_front = Engine2Cal(tmx->getFront());
  CalVector abs_left = Engine2Cal(tmx->getLeft());
  CalQuaternion abs_rot_c = bone_c->getRotationAbsolute();
  CalQuaternion inv_abs_rot_c = abs_rot_c;
  inv_abs_rot_c.invert();
  // These are the local directions that forms the plane of the foot
  CalVector local_front = abs_front;  local_front *= inv_abs_rot_c;
  CalVector local_left  = abs_left;   local_left  *= inv_abs_rot_c;


  //dbg("Delta_y is %f\n", delta_y_over_c);
  //ik.C += XMVectorSet(delta_y_over_c, 0 , 0, 0);
  ik.C += VEC3(0, -distance_of_c_to_ground, 0);


  //ik.normal = XMVector3Cross(ik.C - ik.A, ik.B - ik.A);
  //ik.normal = XMVectorSet(0, -1, 0, 0);
  ik.normal = bm->normal;
  ik.solveB();

  if (amount == 0.f)
    return;

  //// Correct A to point to B
  CCoreModel::TBoneCorrector bc;
  bc.bone_id = bone_id_a;
  bc.local_dir.set(1, 0, 0);
  bc.apply(model, Engine2Cal(ik.B), amount);

  //// Correct B to point to C
  bc.bone_id = bone_id_b;
  bc.local_dir.set(1, 0, 0);
  bc.apply(model, Engine2Cal(ik.C), amount);

  // -------------------------------------------------------------------------
  // Now, that c has been modified, fix the foot orientation

  // Check which are the new two main directions of the ground, sampling the ground
  // at C, and C + front, and C + left
  //TODO OJO CLH TODO_SKEL
  //float distance_of_c0_to_ground = ground.distanceToGround(ik.C);
  //float distance_of_cf_to_ground = ground.distanceToGround(ik.C + Cal2DX(abs_front));
  //float distance_of_cl_to_ground = ground.distanceToGround(ik.C + Cal2DX(abs_left));
  float distance_of_c0_to_ground = 0.5f;
  float distance_of_cf_to_ground = 0.5f;
  float distance_of_cl_to_ground = 0.5f;

  // Fix the front direction, using a bone corrector that will make the old 'local front'
  // look at the new abs front
  CalVector new_abs_front = abs_front;
  new_abs_front.y = distance_of_c0_to_ground - distance_of_cf_to_ground;
  CalVector front_target = bone_c->getTranslationAbsolute() + new_abs_front;
  CCoreModel::TBoneCorrector front_fix(bone_id_c, local_front);
  front_fix.apply(model, front_target, amount);

  // Do the same for the left direction, (which will only apply a roll over the front direction)
  CalVector new_abs_left = abs_left;
  new_abs_left.y = distance_of_c0_to_ground - distance_of_cl_to_ground;
  CalVector left_target = bone_c->getTranslationAbsolute() + new_abs_left;
  CCoreModel::TBoneCorrector left_fix(bone_id_c, local_left);
  left_fix.apply(model, left_target, amount);

}

void TCompSkeletonIK::update(float elapsed) {

  for (int i = 0; i < 2; ++i) {
    if (mods[i].bone_id != -1)
      solveBone(mods + i);
  }

}
