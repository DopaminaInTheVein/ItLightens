#include "mcv_platform.h"
#include "ParticlesEmitter.h"

#include "skeleton\skeleton.h"
#include "components\comp_tags.h"
#include "components\entity.h"
#include "skeleton\comp_skeleton.h"
#include "components\comp_transform.h"

VEC3 TParticlesEmitter::testBones(int num, std::string* name, VEC3* traslacion) {
  CEntity *player = tags_manager.getFirstHavingTag("player");
  assert(player);
  TCompSkeleton *skel_player = player->get<TCompSkeleton>();
  assert(skel_player);

  //auto bone_test = skel_player->model->getSkeleton()->getBone(num);

   //auto bone_test = skel_player->model->getSkeleton()->get

  auto core_vector = skel_player->model->getCoreModel()->getCoreSkeleton()->getVectorRootCoreBoneId();
  auto vector_cal = skel_player->model->getCoreModel()->getCoreSkeleton()->getVectorCoreBone();

  auto bone_test = skel_player->model->getCoreModel()->getCoreSkeleton()->getCoreBone(num);
  auto vector_bone = skel_player->model->getSkeleton()->getVectorBone();
 // auto bone_test = skel_player->model->getSkeleton()->getBone(num);
  *name = bone_test->getName();
  if (!bone_test) {
    return VEC3(0, 0, 0);
  }
  //auto traslacion_cal = bone_test->getTranslation();
  auto traslacion_cal = bone_test->getTranslationAbsolute();
  auto rotation_cal = bone_test->getRotationAbsolute();
  //bone_test-
  TCompTransform* transform = player->get<TCompTransform>();
  *traslacion = VEC3(traslacion_cal.x, traslacion_cal.y, traslacion_cal.z);
  VEC3 rotation = VEC3(rotation_cal.x, rotation_cal.y, rotation_cal.z);
  
  VEC3 position = *traslacion + transform->getPosition();
  VEC3 cardan = transform->getRotation();
  
  //return traslacionEngine;
  return position;
}