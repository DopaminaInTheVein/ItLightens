#include "mcv_platform.h"
#include "resources/resources_manager.h"
#include "comp_skeleton.h"
#include "skeleton.h"
#include "cal3d/cal3d.h"
#include "render/draw_utils.h"
#include "components\comp_transform.h"

#include "imgui/imgui.h"
#include "components\entity.h"

#pragma comment(lib, "cal3d.lib" )

// --------------------------------------------------
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

// --------------------------------------------------
bool TCompSkeleton::load(MKeyValue& atts) {
  std::string res_name = atts.getString("model", "");   // warrior.skeleton
  resource_skeleton = Resources.get(res_name.c_str())->as<CSkeleton>();
  auto non_const_skel = const_cast<CSkeleton*>(resource_skeleton);
  model = new CalModel(non_const_skel->getCoreModel());

  // To get the bones updated right now, otherwis, trying to render will find all bones collapsed in the zero
  model->update(0.f);

  return true;
}

std::string TCompSkeleton::getKeyBoneName(std::string name)
{
  auto non_const_skel = const_cast<CSkeleton*>(resource_skeleton);
  return non_const_skel->getKeyBoneName(name);
}

void TCompSkeleton::onSetAnim(const TMsgSetAnim &msg) {
  //Obtenemos id de la animacion a asignar
  std::vector<std::string> anim_names = msg.name;
  std::vector<std::string> next_loop = msg.nextLoop;
  std::vector<int> nextCycleIds;
  bool prev_anims_cleared = false;

  //Siguiente Loop
  if (next_loop[0] != "") {
    for (auto next : next_loop) {
      int nextCycleId = resource_skeleton->getAnimIdByName(next);
      if (nextCycleId >= 0) {
        nextCycleIds.push_back(nextCycleId);
      }
      //else fatal("Animation %s doesn't exist!", next.c_str());
    }
  }

  //Si no hay siguiente loop, usamos el anterior
  if (nextCycleIds.size() == 0) {
    nextCycleIds = prevCycleIds;
  }

  // activacion nuevas animacinoes
  for (auto name : anim_names) {
    int anim_id = resource_skeleton->getAnimIdByName(name);
    if (anim_id >= 0) {
      if (!prev_anims_cleared) {
        clearPrevAnims(msg.loop);
        prev_anims_cleared = true;
      }

      //Encuentra la animacion con dicho nombre
      //dbg("Cambio anim: %s\n", name.c_str());
      if (msg.loop) {
        //Cycle animation
        model->getMixer()->blendCycle(anim_id, 1.0f, 0.2f);
        prevCycleIds.push_back(anim_id);
      }
      else {
        //Action animation (no loop)
        model->getMixer()->executeAction(anim_id, 0.f, 0.2f, true);
      }
    }
    else {
      //fatal("Animation %s doesn't exist!", name.c_str());
    }
  }

  //Si las nuevas animaciones no son loops, preparamos el siguiente loop
  if (!msg.loop) {
    prevCycleIds = nextCycleIds;
  }
}

void TCompSkeleton::clearPrevAnims(bool isLoop)
{
  // desactivacion de animaciones anteriores
  for (auto prevCycleId : prevCycleIds) {
    if (prevCycleId >= 0) {
      if (isLoop) model->getMixer()->blendCycle(prevCycleId, 0.f, 0.2f);
      else model->getMixer()->blendCycle(prevCycleId, 0.f, 0.0f);
    }
  }
  if (isLoop) prevCycleIds.clear();
}

void TCompSkeleton::renderInMenu() {
  static int anim_id = 0;
  static float in_delay = 0.3f;
  static float out_delay = 0.3f;
  static bool auto_lock = false;
  ImGui::DragInt("Anim Id", &anim_id, 0.1f, 0, model->getCoreModel()->getCoreAnimationCount());
  ImGui::DragFloat("In Delay", &in_delay, 0.1f, 0, 1.f);
  ImGui::DragFloat("Out Delay", &out_delay, 0.1f, 0, 1.f);
  ImGui::Checkbox("Auto lock", &auto_lock);
  if (ImGui::SmallButton("As Cycle")) {
    model->getMixer()->blendCycle(anim_id, 1.0f, in_delay);
  }
  if (ImGui::SmallButton("As Action")) {
    model->getMixer()->executeAction(anim_id, in_delay, out_delay, 1.0f, auto_lock);
  }

  auto mixer = model->getMixer();
  for (auto a : mixer->getAnimationActionList()) {
    ImGui::Text("Action %s S:%d W:%1.2f Time:%1.4f/%1.4f"
      , a->getCoreAnimation()->getName().c_str()
      , a->getState()
      , a->getWeight()
      , a->getTime()
      , a->getCoreAnimation()->getDuration()
      );
  }

  for (auto a : mixer->getAnimationCycle()) {
    ImGui::Text("Cycle %s S:%d W:%1.2f Time:%1.4f"
      , a->getCoreAnimation()->getName().c_str()
      , a->getState()
      , a->getWeight()
      , a->getCoreAnimation()->getDuration()
      );
  }
}

// --------------------------------------------------
void TCompSkeleton::update(float dt) {
  // Transfer our current world location to the cal3d model
  CHandle ownerHandle = CHandle(this).getOwner();
  CEntity* e = ownerHandle;
  if (!e) return;
  if (!isInRoom(ownerHandle)) return;
  TCompTransform* tmx = e->get<TCompTransform>();
  updateEndAction();
  model->getMixer()->extra_trans = Engine2Cal(tmx->getPosition());
  model->getMixer()->extra_rotation = Engine2Cal(tmx->getRotation());
  model->update(dt);
}

void TCompSkeleton::updateEndAction() {
  if (isInRoom(CHandle(this).getOwner())) {
    float endTimeAction = 0.2f; // Tiempo antes de acabar animacion que empieza el blend
    auto mixer = model->getMixer();
    if (mixer->getAnimationActionList().size() == 1) {
      auto lastAction = mixer->getAnimationActionList().front();
      auto lastCoreAction = lastAction->getCoreAnimation();
      float duration = lastCoreAction->getDuration();
      if (duration - lastAction->getTime() < endTimeAction) {
        for (auto prevCycleId : prevCycleIds)
          model->getMixer()->blendCycle(prevCycleId, 1.0f, endTimeAction);
      }
    }
    //if (mixer->getAnimationActionList().size() == 0
    //	&& mixer->getAnimationCycle().size() == 0)
    //{
    //	if (prevCycleId >= 0) {
    //		mixer->blendCycle(prevCycleId, 1.f, 0);
    //	}
    //}
  }
}

void TCompSkeleton::render() const {
  if (isInRoom(CHandle(this).getOwner())) {
    auto skel = model->getSkeleton();
    size_t nbones = skel->getVectorBone().size();
    std::vector< VEC3 > bone_points;
    bone_points.resize(nbones * 2); // begin to end
    int nlines = skel->getBoneLines(&bone_points[0].x);
    float scale = 1.0f;
    for (int i = 0; i < nlines; ++i) {
      Debug->DrawLine(bone_points[i * 2] * scale, bone_points[i * 2 + 1] * scale, VEC3(1, 0, 1));
    }
  }
}

void TCompSkeleton::uploadBonesToCteShader() const {
  //if (isInRoom(CHandle(this).getOwner())) {
  float* fout = &shader_ctes_bones.Bones[0]._11;

  CalSkeleton* skel = model->getSkeleton();
  auto& cal_bones = skel->getVectorBone();
  assert(cal_bones.size() < MAX_BONES_SUPPORTED);
  // For each bone from the cal model
  for (size_t bone_idx = 0; bone_idx < cal_bones.size(); ++bone_idx) {
    CalBone* bone = cal_bones[bone_idx];

    const CalMatrix& cal_mtx = bone->getTransformMatrix();
    const CalVector& cal_pos = bone->getTranslationBoneSpace();

    *fout++ = cal_mtx.dxdx;
    *fout++ = cal_mtx.dydx;
    *fout++ = cal_mtx.dzdx;
    *fout++ = 0.f;
    *fout++ = cal_mtx.dxdy;
    *fout++ = cal_mtx.dydy;
    *fout++ = cal_mtx.dzdy;
    *fout++ = 0.f;
    *fout++ = cal_mtx.dxdz;
    *fout++ = cal_mtx.dydz;
    *fout++ = cal_mtx.dzdz;
    *fout++ = 0.f;
    *fout++ = cal_pos.x;
    *fout++ = cal_pos.y;
    *fout++ = cal_pos.z;
    *fout++ = 1.f;
  }

  shader_ctes_bones.uploadToGPU();
  //}
  // Already done in game.cpp
  // shader_ctes_bones.activate(CTE_SHADER_BONES_SLOT);
}