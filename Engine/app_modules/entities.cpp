#include "mcv_platform.h"
#include "app_modules/entities.h"
#include "handle/handle_manager.h"
#include "components/components.h"
#include "components/entity_parser.h"
#include "handle/msgs.h"
#include "components/comp_msgs.h"
#include "components/entity_tags.h"
#include "render/technique.h"
#include "resources/resources_manager.h"
#include "imgui/imgui.h"

DECL_OBJ_MANAGER("entity", CEntity);
DECL_OBJ_MANAGER("name", TCompName);
DECL_OBJ_MANAGER("transform", TCompTransform);
DECL_OBJ_MANAGER("camera", TCompCamera);
DECL_OBJ_MANAGER("controller_3rd_person", TCompController3rdPerson);
DECL_OBJ_MANAGER("controller_1st_person", TCompController1stPerson);
DECL_OBJ_MANAGER("render_static_mesh", TCompRenderStaticMesh);
DECL_OBJ_MANAGER("hierarchy", TCompHierarchy);
DECL_OBJ_MANAGER("skeleton", TCompSkeleton);

DECL_OBJ_MANAGER("life", TCompLife);

// The global dict of all msgs
MMsgSubscriptions msg_subscriptions;
TMsgID generateUniqueMsgID() {
  static TMsgID next_msg_id = 1;
  return next_msg_id++;
}

bool CEntitiesModule::start() {
  uint32_t nmax = 64;
  getHandleManager<CEntity>()->init(nmax);
  getHandleManager<TCompName>()->init(nmax);
  getHandleManager<TCompTransform>()->init(nmax);
  getHandleManager<TCompRenderStaticMesh>()->init(nmax);
  getHandleManager<TCompCamera>()->init(4);
  getHandleManager<TCompController3rdPerson>()->init(4);
  getHandleManager<TCompController1stPerson>()->init(4);
  getHandleManager<TCompHierarchy>()->init(nmax);
  getHandleManager<TCompSkeleton>()->init(nmax);

  getHandleManager<TCompLife>()->init(nmax);

  SUBSCRIBE(TCompLife, TMsgDamage, onDamage);
  SUBSCRIBE(TCompLife, TMsgEntityCreated, onCreate);
  SUBSCRIBE(TCompTransform, TMsgEntityCreated, onCreate);
  SUBSCRIBE(TCompController3rdPerson, TMsgSetTarget, onSetTarget);
  SUBSCRIBE(TCompRenderStaticMesh, TMsgEntityCreated, onCreate);

  {
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
  }

  {
    CEntityParser ep;
    bool is_ok = ep.xmlParseFile("data/scenes/scene01.xml");
    assert(is_ok);
  }
  return true;
}

void CEntitiesModule::stop() {
}

void CEntitiesModule::update(float dt) {

  getHandleManager<TCompController3rdPerson>()->updateAll(dt);
  getHandleManager<TCompController1stPerson>()->updateAll( dt );
  getHandleManager<TCompCamera>()->updateAll(dt);
  getHandleManager<TCompSkeleton>()->updateAll( dt );
  
  // Move this line to the physics module maybe?
  // Physics.get()..update( dt );
  //getHandleManager<TCompPhysics>()->onAll(&TCompPhysics::updateFromPhysics);

  // Show a menu to modify any entity
  renderInMenu();

  if (io->keys[ 'A' ].becomesPressed()) {
    dbg("Key A pressed!\n");
  }
  if (io->keys['A'].becomesReleased()) {
    dbg("Key A Released after %f sec!\n" ,io->keys['A'].timePressed());
  }

}

void CEntitiesModule::render() {
  // for each manager
  // if manager has debug render active
  // manager->renderAll()

  auto tech = Resources.get("solid_colored.tech")->as<CRenderTechnique>();
  // All components render in this stage with this 'tech'
  tech->activate();
  getHandleManager<TCompTransform>()->onAll(&TCompTransform::render);
  getHandleManager<TCompCamera>()->onAll(&TCompCamera::render);
  getHandleManager<TCompSkeleton>()->onAll( &TCompSkeleton::render );
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