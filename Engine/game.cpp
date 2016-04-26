#include "mcv_platform.h"
#include "windows/app.h"
#include "render/mesh.h"
#include "render/shaders.h"
#include "render/vertex_declarations.h"
#include "render/shader_cte.h"
#include "render/technique.h"
#include "render/texture.h"
#include "render/render_manager.h"
#include "render/draw_utils.h"
#include "resources/resources_manager.h"
#include "camera/camera.h"
#include "app_modules/app_module.h"
#include "app_modules/imgui/module_imgui.h"
#include "app_modules/gui/gui.h"
#include "input/input.h"
#include "app_modules/io/io.h"
#include "app_modules/logic_manager/logic_manager.h"
#include "app_modules/sound_manager/sound_manager.h"

#include "app_modules/render/module_render_deferred.h"
#include "components/entity_parser.h"
#include "handle/handle_manager.h"
#include "utils/directory_watcher.h"

#include <shellapi.h>
#include <process.h>

const CRenderTechnique* tech_solid_colored = nullptr;
const CRenderTechnique* tech_textured_colored = nullptr;
CDirectoyWatcher resources_dir_watcher;

// --------------------------------------------
#include "app_modules/entities.h"

//DEBUG
CDebug *	  Debug = nullptr;
CUI ui;
CGameController* GameController = nullptr;
CPhysxManager *g_PhysxManager = nullptr;

// --------------------------------------------

bool CApp::start() {

	// imgui must be the first to update and the last to render
	auto imgui = new CImGuiModule;
	auto gui = new CGuiModule;
	auto entities = new CEntitiesModule;
	auto render_deferred = new CRenderDeferredModule;
	io = new CIOModule;     // It's the global io
	g_PhysxManager = new CPhysxManager;
	GameController = new CGameController;
	Debug = new CDebug();
	logic_manager = new CLogicManagerModule;
	sound_manager = new CSoundManagerModule;


	// Will contain all modules created
	all_modules.push_back(imgui);
	all_modules.push_back(gui);
	all_modules.push_back(g_PhysxManager);
	all_modules.push_back(entities);
	all_modules.push_back(io);
	all_modules.push_back(GameController);
	all_modules.push_back(Debug);
	all_modules.push_back(render_deferred);
	all_modules.push_back(logic_manager);
	all_modules.push_back(sound_manager);

	mod_update.push_back(GameController);
	mod_update.push_back(imgui);
	mod_update.push_back(gui);
	mod_update.push_back(entities);
	mod_update.push_back(g_PhysxManager);
	mod_update.push_back(io);
	mod_update.push_back(Debug);
	mod_update.push_back(logic_manager);
	
	mod_renders.push_back(render_deferred);
	mod_renders.push_back(entities);
	mod_renders.push_back(Debug);
	mod_renders.push_back(gui);
	mod_renders.push_back(imgui);

	mod_renders.push_back(io);

	mod_init_order.push_back(gui);
	mod_init_order.push_back(imgui);
	mod_init_order.push_back(render_deferred);
	mod_init_order.push_back(io);
	mod_init_order.push_back(g_PhysxManager);
	mod_init_order.push_back(entities);
	mod_init_order.push_back(logic_manager);
	mod_init_order.push_back(sound_manager);

	mod_wnd_proc.push_back(io);
	mod_wnd_proc.push_back(imgui);

// ----------------------------
  if (!drawUtilsCreate())
    return false;

resources_dir_watcher.start("data/shaders", getHWnd());

	// ----------------------------
	tech_solid_colored = Resources.get("solid_colored.tech")->as<CRenderTechnique>();
	tech_textured_colored = Resources.get("textured.tech")->as<CRenderTechnique>();

	if (!shader_ctes_camera.create("ctes_camera"))
		return false;
	if (!shader_ctes_object.create("ctes_object"))
		return false;
	if (!shader_ctes_bones.create("ctes_bones"))
		return false;
	shader_ctes_bones.activate(CTE_SHADER_BONES_SLOT);
	// Init modules
	for (auto it : mod_init_order) {
		if (!it->start()) {
			dbg("Failed to init module %s\n", it->getName());
			return false;
		}
	}

	io->mouse.toggle();

	GameController->SetGameState(CGameController::RUNNING);

	logic_manager->throwEvent(logic_manager->OnGameStart, "");

	return true;
}

// ----------------------------------
void CApp::stop() {
	// Stop modules
	for (auto it = mod_init_order.rbegin(); it != mod_init_order.rend(); ++it)
		(*it)->stop();

	Resources.destroy();

	shader_ctes_bones.destroy();
	shader_ctes_camera.destroy();
	shader_ctes_object.destroy();

	// Delete all modules
	for (auto m : all_modules)
		delete m;
	all_modules.clear();
}

void CApp::restart() {
	HWND hTempWnd = getHWnd();
	char szFileName[MAX_PATH] = "";
	GetModuleFileName(NULL, szFileName, MAX_PATH);
	ShellExecute(GetDesktopWindow(), "open", szFileName, NULL, NULL, SW_SHOWDEFAULT);
	SendMessage(hTempWnd, WM_CLOSE, 0, 0);
}

void CApp::exitGame() {
	HWND hTempWnd = getHWnd();
	SendMessage(hTempWnd, WM_CLOSE, 0, 0);
}

// ----------------------------------
void CApp::update(float elapsed) {
	PROFILE_FUNCTION("update");
	for (auto it : mod_update) {
		PROFILE_FUNCTION(it->getName());
		it->update(elapsed);
	}
	static float ctime = 0.f;
	ctime += elapsed* 0.01f;

	CHandleManager::destroyAllPendingObjects();
}

// ----------------------------------
void CApp::render() {
  PROFILE_FUNCTION("CApp::render");
  
  activateDefaultStates();

  for (auto it : mod_renders) {
    PROFILE_FUNCTION(it->getName());
    CTraceScoped scope( it->getName() );
    it->render();
  }

}



