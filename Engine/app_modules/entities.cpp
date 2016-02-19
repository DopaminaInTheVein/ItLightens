#include "mcv_platform.h"
#include "app_modules/entities.h"
#include "handle/handle_manager.h"
#include "components/components.h"
#include "components/entity_parser.h"
#include "handle/msgs.h"
#include "components/comp_msgs.h"
#include "components/entity_tags.h"
#include "imgui/imgui.h"

DECL_OBJ_MANAGER("entity", CEntity);
DECL_OBJ_MANAGER("name", TCompName);
DECL_OBJ_MANAGER("transform", TCompTransform);
DECL_OBJ_MANAGER("camera", TCompCamera);
DECL_OBJ_MANAGER("controller_3rd_person", TCompController3rdPerson);

DECL_OBJ_MANAGER("life", TCompLife);

// The global dict of all msgs
MMsgSubscriptions msg_subscriptions;
TMsgID generateUniqueMsgID() {
  static TMsgID next_msg_id = 1;
  return next_msg_id++;
}

bool CEntitiesModule::start() {
  uint32_t nmax = 8;
  getHandleManager<CEntity>()->init(nmax);
  getHandleManager<TCompName>()->init(nmax);
  getHandleManager<TCompTransform>()->init(nmax);
  getHandleManager<TCompCamera>()->init(4);
  getHandleManager<TCompController3rdPerson>()->init(4);

  getHandleManager<TCompLife>()->init(nmax);

  SUBSCRIBE(TCompLife, TMsgDamage, onDamage);
  SUBSCRIBE(TCompLife, TMsgEntityCreated, onCreate);
  SUBSCRIBE(TCompTransform, TMsgEntityCreated, onCreate);
  SUBSCRIBE(TCompController3rdPerson, TMsgSetTarget, onSetTarget);

  CEntityParser ep;
  bool is_ok = ep.xmlParseFile("data/scenes/scene00.xml");
  assert(is_ok);

  CEntity* e = tags_manager.getFirstHavingTag(getID("player"));
  CHandle t = tags_manager.getFirstHavingTag(getID("target"));
  if (e && t.isValid()) {
    TMsgSetTarget msg;
    msg.target = t;
    e->sendMsg(msg);
  }


  return true;
}

void CEntitiesModule::stop() {
}

void CEntitiesModule::update(float dt) {

  getHandleManager<TCompController3rdPerson>()->updateAll( dt );
  getHandleManager<TCompCamera>()->updateAll( dt );
  
  // Show a menu to modify any entity
  renderInMenu();
}

void CEntitiesModule::render() {
  // for each manager
  // if manager has debug render active
  // manager->renderAll()
  getHandleManager<TCompTransform>()->onAll(&TCompTransform::render);
  getHandleManager<TCompCamera>()->onAll( &TCompCamera::render );
  //getHandleManager<TCompLife>()->renderAll();
}

void CEntitiesModule::renderInMenu() {
  ImGui::Begin("Entities");
  if (ImGui::TreeNode("All entities...")) {
    getHandleManager<CEntity>()->onAll(&CEntity::renderInMenu);
    ImGui::TreePop();
  } 
  if (ImGui::TreeNode("Entities by Tag...")) {
    // Show all defined tags
    ImGui::TreePop();
  }
  ImGui::End();
}