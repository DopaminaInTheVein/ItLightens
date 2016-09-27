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
#include "app_modules/io/io.h"
#include "app_modules/logic_manager/logic_manager.h"
#include "app_modules/sound_manager/sound_manager.h"
#include "app_modules/lang_manager/lang_manager.h"
#include "particles\particles_manager.h"

#include "app_modules/render/module_render_deferred.h"
#include "components/entity_parser.h"
#include "handle/handle_manager.h"
#include "utils/directory_watcher.h"

#include "app_modules/navmesh/navmesh_manager.h"

#include <shellapi.h>
#include <process.h>

using namespace std;

const CRenderTechnique* tech_solid_colored = nullptr;
const CRenderTechnique* tech_textured_colored = nullptr;
CDirectoyWatcher resources_dir_watcher;

// --------------------------------------------
#include "app_modules/entities.h"
CEntitiesModule * entities = nullptr;

//DEBUG
CDebug *	  Debug = nullptr;
CUI ui;
CGameController* GameController = nullptr;
CPhysxManager *g_PhysxManager = nullptr;
CGuiModule * Gui = nullptr;
CRenderDeferredModule * render_deferred;
// --------------------------------------------

bool CApp::start() {
	map<std::string, std::string> fields = readIniAtrDataStr(file_options_json, "scenes");
	//next_level = fields["first_scene"];
	map<std::string, std::string> lang_map = readIniAtrDataStr(CApp::get().file_options_json, "language");

	// imgui must be the first to update and the last to render
	auto imgui = new CImGuiModule;
	Gui = new CGuiModule;
	entities = new CEntitiesModule;
	render_deferred = new CRenderDeferredModule;
	io = new CIOModule;     // It's the global io
	g_PhysxManager = new CPhysxManager;
	g_particlesManager = new CParticlesManager;
	GameController = new CGameController;
	Debug = new CDebug();
	logic_manager = new CLogicManagerModule;
	sound_manager = new CSoundManagerModule;
	lang_manager = new CLangManagerModule(lang_map["lang"]);

	// Will contain all modules created
	all_modules.push_back(imgui);
	all_modules.push_back(Gui);
	all_modules.push_back(g_PhysxManager);
	all_modules.push_back(g_particlesManager);
	all_modules.push_back(entities);
	all_modules.push_back(io);
	all_modules.push_back(GameController);
	all_modules.push_back(Debug);
	all_modules.push_back(render_deferred);
	all_modules.push_back(logic_manager);
	all_modules.push_back(sound_manager);
	all_modules.push_back(lang_manager);

	mod_update.push_back(lang_manager);
	mod_update.push_back(logic_manager);
	mod_update.push_back(sound_manager);
	mod_update.push_back(entities);
	mod_update.push_back(g_particlesManager);
	mod_update.push_back(GameController);
	mod_update.push_back(imgui);
	mod_update.push_back(render_deferred);
	mod_update.push_back(Gui);
	mod_update.push_back(g_PhysxManager);
	mod_update.push_back(io);
	mod_update.push_back(Debug);

	mod_renders.push_back(render_deferred);
	mod_renders.push_back(entities);
	mod_renders.push_back(Debug);
	mod_renders.push_back(g_particlesManager);
	mod_renders.push_back(Gui);
	mod_renders.push_back(imgui);

	mod_renders.push_back(io);

	mod_init_order.push_back(render_deferred);
	mod_init_order.push_back(GameController);
	mod_init_order.push_back(Gui);
	mod_init_order.push_back(imgui);
	mod_init_order.push_back(io);
	mod_init_order.push_back(g_PhysxManager);
	mod_init_order.push_back(g_particlesManager);   //need to be initialized before the entities
	mod_init_order.push_back(logic_manager);
	mod_init_order.push_back(sound_manager);
	mod_init_order.push_back(lang_manager);
	mod_init_order.push_back(entities);

	mod_wnd_proc.push_back(io);
	mod_wnd_proc.push_back(imgui);

	// ----------------------------
	if (!drawUtilsCreate())
		return false;

	resources_dir_watcher.start("data", getHWnd());

	// ----------------------------
	tech_solid_colored = Resources.get("solid_colored.tech")->as<CRenderTechnique>();
	tech_textured_colored = Resources.get("textured.tech")->as<CRenderTechnique>();

	// Init modules
	for (auto it : mod_init_order) {
		CLog::appendFormat("Starting module %s...", it->getName());
		if (!it->start()) {
			dbg("Failed to init module [%s]\n", it->getName());
			CLog::appendFormat("ERROR!! Failed to init module %s\n", it->getName());
			return false;
		}
		CLog::appendFormat("Module [%s] started successfully.", it->getName());
	}

	io->mouse.toggle();

	imgui->StartLightEditor(); //need to be created after entities

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
	drawUtilsDestroy();

	getHandleManager<TCompLightDirShadows>()->each([](TCompLightDirShadows* c) {
		c->destroy();
	});

	// Delete all modules
	for (auto m : all_modules)
		delete m;
	all_modules.clear();
	mod_update.clear();
	mod_renders.clear();
	mod_init_order.clear();
	mod_wnd_proc.clear();
}

//----------------------------------
void CApp::changeScene(string level) {
	dbg("Destroying scene...\n");
	// Level load
	bool reload = level == getCurrentLogicLevel();
	entities->clear(reload);
	next_level = level;
}
void CApp::loadEntities(string file_name) {
	CEntitiesModule::ParsingInfo info;
	info.filename = file_name;
	info.reload = false;
	entities->loadXML(info);
}

//void CApp::restart() {
//	HWND hTempWnd = getHWnd();
//	char szFileName[MAX_PATH] = "";
//	GetModuleFileName(NULL, szFileName, MAX_PATH);
//	ShellExecute(GetDesktopWindow(), "open", szFileName, NULL, NULL, SW_SHOWDEFAULT);
//	SendMessage(hTempWnd, WM_CLOSE, 0, 0);
//}

void CApp::restartLevel() {
	changeScene(current_level);
}

std::string CApp::getCurrentRealLevel() {
	return getRealLevel(current_level);
}

std::string CApp::getCurrentLogicLevel() {
	return current_level;
}

std::string CApp::getRealLevel(std::string logic_level) {
	map<std::string, std::string> fields = readIniAtrDataStr(CApp::get().file_options_json, "scenes");
	return fields[logic_level];
}

void CApp::restartLevelNotify() {
	char params[128];
	sprintf(params, "\"%s\", \"%s\"", getCurrentLogicLevel().c_str(), getCurrentRealLevel().c_str());
	logic_manager->throwEvent(CLogicManagerModule::EVENT::OnRestartLevel, std::string(params));
}

void CApp::saveLevel() {
	has_check_point.insert(getCurrentLogicLevel());
	entities->saveLevel(getCurrentRealLevel());
	char params[128];
	sprintf(params, "\"%s\", \"%s\"", getCurrentLogicLevel().c_str(), getCurrentRealLevel().c_str());
	logic_manager->throwEvent(CLogicManagerModule::EVENT::OnSavedLevel, std::string(params));
}

void CApp::clearSaveData() {
	has_check_point.clear();
}

void CApp::loadedLevelNotify() {
	current_level = next_level;
	next_level = "";
	char params[128];
	sprintf(params, "\"%s\", \"%s\"", getCurrentLogicLevel().c_str(), getCurrentRealLevel().c_str());
	auto game_event = setContains(has_check_point, getCurrentLogicLevel())
		? CLogicManagerModule::EVENT::OnLoadedLevel
		: CLogicManagerModule::EVENT::OnLevelStart;

	logic_manager->throwEvent(game_event, std::string(params));
	loading = false;
	if (!GameController->IsUiControl())
		GameController->SetGameState(CGameController::RUNNING);
}

void CApp::exitGame() {
	HWND hTempWnd = getHWnd();
	SendMessage(hTempWnd, WM_CLOSE, 0, 0);
}

// ----------------------------------
void CApp::update(float elapsed) {
	PROFILE_FUNCTION("update");
	for (auto it : mod_update) {
		if (GameController->GetGameState() == CGameController::RUNNING) {
			PROFILE_FUNCTION(it->getName());
			auto name = it->getName();
			it->update(elapsed);
		}
		else if (it->forcedUpdate()) {
			PROFILE_FUNCTION(it->getName());
			it->update(getDeltaTime(1.0f));
		}
	}
	static float ctime = 0.f;
	ctime += elapsed* 0.01f;
	CHandleManager::destroyAllPendingObjects();
	if (next_level != "" && entities->isCleared()) {
		if (!loading) {
			// Loading state and screen
			loading = true;
			GameController->LoadComplete(false);
			showLoadingScreen();
			initNextLevel();
		}
	}
}

void CApp::initNextLevel()
{
	std::vector<CEntitiesModule::ParsingInfo> files_to_parse;
	CEntityCounter entity_counter;

	// Restart Timers LUA
	logic_manager->resetTimers();

	// Stop sounds
	sound_manager->stopAllSounds();

	//
	std::string level_name = getRealLevel(next_level);
	bool reload = next_level == current_level;
	if (!reload) CEntityParser::clearCollisionables();
	bool is_ok;
	//SetLoadingState(5);

	// Entidades invariantes
	CEntitiesModule::ParsingInfo info;
	info.filename = level_name;
	info.reload = reload;
	files_to_parse.push_back(info);
	entity_counter.xmlParseFile("data/scenes/" + info.filename + ".xml");

	// Entidades variantes
	info.filename = level_name + (setContains(has_check_point, next_level) ? "_save" : "_init");
	files_to_parse.push_back(info);
	entity_counter.xmlParseFile("data/scenes/" + info.filename + ".xml");

	// Lights
	info.filename = level_name + "_lights";
	files_to_parse.push_back(info);
	entity_counter.xmlParseFile("data/scenes/" + info.filename + ".xml");

	CEntityParser::setNumEntities(entity_counter.getNumEntities());

	// Load entities
	for (auto file : files_to_parse) {
		file.loading_control = true;
		entities->loadXML(file);
	}

	// Init entities
	entities->initEntities();
	//SetLoadingState(80);

	// Navmesh
	if (!reload) CNavmeshManager::initNavmesh(level_name);
	GameController->SetLoadingState(100.f);

	// Game state and notify
	loadedLevelNotify();
}

void CApp::showLoadingScreen()
{
	// Restart Timers LUA
	logic_manager->resetTimers();

	//
	std::string level_name = "loading";
	bool reload = next_level == current_level;
	if (!reload) CEntityParser::clearCollisionables();
	bool is_ok;

	// Entidades invariantes
	CEntitiesModule::ParsingInfo info;
	info.filename = level_name;
	info.reload = reload;
	is_ok = entities->loadXML(info);
	assert(is_ok);

	// Init entities
	entities->initEntities();
}

// ----------------------------------
void CApp::render() {
	PROFILE_FUNCTION("CApp::render");
	activateDefaultStates();

	for (auto it : mod_renders) {
		PROFILE_FUNCTION(it->getName());
		CTraceScoped scope(it->getName());
		it->render();
	}
}

int CApp::getXRes(bool ask_window) {
	if (!ask_window && render_deferred) {
		return render_deferred->getXRes();
	}
	if (!max_screen)
		return xres;
	else
		return xres_max;
}
int CApp::getYRes(bool ask_window) {
	if (!ask_window && render_deferred) {
		return render_deferred->getYRes();
	}
	if (!max_screen)
		return yres;
	else
		return yres_max;
}