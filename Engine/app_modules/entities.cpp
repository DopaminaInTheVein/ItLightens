#include "mcv_platform.h"
#include "handle/handle.h"
#include "app_modules/entities.h"
#include "components/components.h"
#include "components/entity_parser.h"
#include "handle/msgs.h"
#include "components/comp_msgs.h"
#include "components/entity_tags.h"
#include "render/technique.h"
#include "resources/resources_manager.h"

DECL_OBJ_MANAGER("entity", CEntity);
DECL_OBJ_MANAGER("name", TCompName);
DECL_OBJ_MANAGER("transform", TCompTransform);
DECL_OBJ_MANAGER("camera", TCompCamera);
DECL_OBJ_MANAGER("controller_3rd_person", TCompController3rdPerson);
DECL_OBJ_MANAGER("controller_1st_person", TCompController1stPerson);
DECL_OBJ_MANAGER("render_static_mesh", TCompRenderStaticMesh);
DECL_OBJ_MANAGER("hierarchy", TCompHierarchy);
DECL_OBJ_MANAGER("skeleton", TCompSkeleton);
DECL_OBJ_MANAGER("bone_tracker", TCompBoneTracker);
DECL_OBJ_MANAGER("abs_aabb", TCompAbsAABB);
DECL_OBJ_MANAGER("local_aabb", TCompLocalAABB);
DECL_OBJ_MANAGER("culling", TCompCulling);
DECL_OBJ_MANAGER("light_dir", TCompLightDir);
DECL_OBJ_MANAGER("light_dir_shadows", TCompLightDirShadows);
DECL_OBJ_MANAGER("tags", TCompTags);
DECL_OBJ_MANAGER("light_point", TCompLightPoint);
<<<<<<< HEAD
=======
DECL_OBJ_MANAGER("render_glow", TCompRenderGlow);
>>>>>>> d6b4e6803fa82f01d5c091b986f30dbebbb8b427

DECL_OBJ_MANAGER("life", TCompLife);

// The global dict of all msgs
MMsgSubscriptions msg_subscriptions;
TMsgID generateUniqueMsgID() {
	static TMsgID next_msg_id = 1;
	return next_msg_id++;
}

bool CEntitiesModule::start() {
	uint32_t nmax = 512;
	getHandleManager<CEntity>()->init(nmax);
	getHandleManager<TCompName>()->init(nmax);
	getHandleManager<TCompTransform>()->init(nmax);
	getHandleManager<TCompRenderStaticMesh>()->init(nmax);
	getHandleManager<TCompController3rdPerson>()->init(4);
	getHandleManager<TCompController1stPerson>()->init(4);
  getHandleManager<TCompHierarchy>()->init(nmax);
  getHandleManager<TCompAbsAABB>()->init(nmax);
  getHandleManager<TCompLocalAABB>()->init(nmax);
	getHandleManager<TCompSkeleton>()->init(nmax);
	getHandleManager<TCompBoneTracker>()->init(nmax);
	getHandleManager<TCompTags>()->init(nmax);

	getHandleManager<TCompCamera>()->init(4);
  getHandleManager<TCompCulling>()->init(4);
  getHandleManager<TCompLightDir>()->init(4);
  getHandleManager<TCompLightDirShadows>()->init(4);
<<<<<<< HEAD
	getHandleManager<TCompLightPoint>()->init(32);
=======
  getHandleManager<TCompLightPoint>()->init(32);
  getHandleManager<TCompRenderGlow>()->init(4);
>>>>>>> d6b4e6803fa82f01d5c091b986f30dbebbb8b427

	getHandleManager<TCompLife>()->init(nmax);

	SUBSCRIBE(TCompLife, TMsgDamage, onDamage);
	SUBSCRIBE(TCompLife, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompTransform, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompController3rdPerson, TMsgSetTarget, onSetTarget);
	SUBSCRIBE(TCompRenderStaticMesh, TMsgEntityCreated, onCreate);
  SUBSCRIBE(TCompRenderStaticMesh, TMsgGetLocalAABB, onGetLocalAABB);
  SUBSCRIBE(TCompHierarchy, TMsgEntityGroupCreated, onGroupCreated);
  SUBSCRIBE(TCompBoneTracker, TMsgEntityGroupCreated, onGroupCreated);
  SUBSCRIBE(TCompAbsAABB, TMsgEntityCreated, onCreate);
  SUBSCRIBE(TCompLocalAABB, TMsgEntityCreated, onCreate);
  SUBSCRIBE(TCompTags, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompTags, TMsgAddTag, onTagAdded);
  SUBSCRIBE(TCompCamera, TMsgGetCullingViewProj, onGetViewProj);

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
    bool is_ok = ep.xmlParseFile("data/scenes/scene_pbr.xml");
    //ol is_ok = ep.xmlParseFile("data/scenes/scene_normal_map.xml");
		assert(is_ok);
	}
<<<<<<< HEAD
  {
    CEntityParser ep;
    bool is_ok = ep.xmlParseFile("data/scenes/scene_basic_lights.xml");
=======
  {
    CEntityParser ep;
    bool is_ok = ep.xmlParseFile("data/scenes/scene_basic_lights.xml");
    assert(is_ok);
  }
  {
    CEntityParser ep;
    bool is_ok = ep.xmlParseFile("data/scenes/scene_ui.xml");
>>>>>>> d6b4e6803fa82f01d5c091b986f30dbebbb8b427
    assert(is_ok);
  }
  //{
  //  CEntityParser ep;
  //  bool is_ok = ep.xmlParseFile("data/scenes/scene_aabbs.xml");
  //  assert(is_ok);
  //}
	return true;
}

void CEntitiesModule::stop() {
}

void CEntitiesModule::update(float dt) {
	getHandleManager<TCompController3rdPerson>()->updateAll(dt);
	getHandleManager<TCompController1stPerson>()->updateAll(dt);
	getHandleManager<TCompCamera>()->updateAll(dt);
	getHandleManager<TCompLightDir>()->updateAll(dt);
  getHandleManager<TCompLightDirShadows>()->updateAll(dt);

	if (use_parallel)
		getHandleManager<TCompSkeleton>()->updateAllInParallel(dt);
	else
		getHandleManager<TCompSkeleton>()->updateAll(dt);

	getHandleManager<TCompBoneTracker>()->updateAll(dt);
  getHandleManager<TCompHierarchy>()->onAll(&TCompHierarchy::updateWorldFromLocal);
  getHandleManager<TCompLocalAABB>()->onAll(&TCompLocalAABB::updateAbs);
  getHandleManager<TCompCulling>()->onAll(&TCompCulling::update);

	// Move this line to the physics module maybe?
	// Physics.get()..update( dt );
	//getHandleManager<TCompPhysics>()->onAll(&TCompPhysics::updateFromPhysics);

	// Show a menu to modify any entity
	renderInMenu();

	/*
	if (io->keys[ 'T' ].becomesPressed()) {
	  dbg("Key T pressed!\n");
	  // Sending msg 'addTag' to all defined entities
	  TMsgAddTag m = { 0x123456 };
	  getHandleManager<CEntity>()->each(  [&m](CEntity* e) {
		e->sendMsg(m);
	  });
	}

	if (io->keys['Y'].becomesPressed()) {
	  dbg("Key Y pressed!\n");
	  // Sending msg 'addTag' to all defined entities
	  TMsgAddTag m = { 0x123456 };
	  for (auto h : tags_manager.getHandlesByTag(getID("bullet"))) {
		h.sendMsg( m );
	  }
	}

	if (io->keys['T'].becomesReleased()) {
	  dbg("Key A Released after %f sec!\n" ,io->keys['A'].timePressed());
	}
	*/
}

void CEntitiesModule::render() {
	auto tech = Resources.get("solid_colored.tech")->as<CRenderTechnique>();
	// All components render in this stage with this 'tech'
	tech->activate();
	getHandleManager<TCompTransform>()->onAll(&TCompTransform::render);
	getHandleManager<TCompCamera>()->onAll(&TCompCamera::render);
	getHandleManager<TCompLightDir>()->onAll(&TCompLightDir::render);
  getHandleManager<TCompLightDirShadows>()->onAll(&TCompLightDirShadows::render);
  getHandleManager<TCompSkeleton>()->onAll(&TCompSkeleton::render);
  getHandleManager<TCompAbsAABB>()->onAll(&TCompAbsAABB::render);
  getHandleManager<TCompLocalAABB>()->onAll(&TCompLocalAABB::render);
<<<<<<< HEAD
	//getHandleManager<TCompLife>()->renderAll();
=======

  RenderManager.renderAll(CHandle(), CRenderTechnique::DBG_OBJS);
  RenderManager.renderAll( CHandle(), CRenderTechnique::UI_OBJS);

>>>>>>> d6b4e6803fa82f01d5c091b986f30dbebbb8b427
}

void CEntitiesModule::renderInMenu() {
	ImGui::Begin("Entities");
	if (ImGui::TreeNode("All entities...")) {
		getHandleManager<CEntity>()->onAll(&CEntity::renderInMenu);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Entities by Tag...")) {
		tags_manager.renderInMenu();
		// Show all defined tags
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Pitufos...")) {
		static int num_pitufos = 0;
		static int delta = 10;
		static int row = 0;
		ImGui::Text("We have %d pitufos", num_pitufos);
		ImGui::Checkbox("In Parallel", &use_parallel);
		ImGui::InputInt("Pitufos per row", &delta, 1, 10);
		if (ImGui::SmallButton("Add 10")) {
			for (int i = 0; i < delta; ++i) {
				CEntity* e = createPrefab("pitufo");
				if (e) {
					TCompTransform* t = e->get<TCompTransform>();
					t->setPosition(VEC3(float(row), 0.f, float(i)));
					TCompName* n = e->get<TCompName>();
					sprintf(n->name, "Pitufo %d", num_pitufos);
					t->setPosition(VEC3(float(row), 0.f, float(i)));
					++num_pitufos;
				}
			}
			++row;
		}
		// Show all defined tags
		ImGui::TreePop();
	}

	//if (io->keys['Y'].becomesPressed()) {
	//  dbg("Y becomes pressed\n");
	//}
	//if (io->keys['Y'].becomesReleased()) {
	//  dbg("Y becomes realeased\n");
	//}
	//if (io->keys['Y'].isPressed()) {
	//  ImGui::Text("Y Pressed for %f secs", io->keys['Y'].timePressed());
	//}
	ImGui::End();
}