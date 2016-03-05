#include "mcv_platform.h"
#include "windows/app.h"
#include "render/mesh.h"
#include "render/shaders.h"
#include "render/vertex_declarations.h"
#include "render/shader_cte.h"
#include "render/technique.h"
#include "render/texture.h"
#include "render/render_manager.h"
#include "resources/resources_manager.h"
#include "camera/camera.h"
#include "app_modules/app_module.h"
#include "app_modules/imgui/module_imgui.h"
#include "input/input.h"
#include "app_modules/io/io.h"

#include "logic/sbb.h"
#include "handle/object_manager.h"
#include "components/comp_transform.h"
#include "components/entity_tags.h"
#include "components/entity.h"

#include "debug/debug.h"

#include "physics/simple_physx.h"
#include "ui/ui_interface.h"

#include <shellapi.h>
#include <process.h>

//DEBUG
CDebug *	  Debug = nullptr;
CSimplePhysx  s_physx;		//provisional
CUI ui;

const CRenderTechnique* tech_solid_colored = nullptr;
const CRenderTechnique* tech_textured_colored = nullptr;
const CTexture*         texture1 = nullptr;

#include "contants/ctes_camera.h"
CShaderCte< TCteCamera > shader_ctes_camera;
#include "contants/ctes_object.h"
CShaderCte< TCteObject > shader_ctes_object;

// --------------------------------------------
#include "app_modules/entities.h"

bool CApp::start() {

	//light setup init
	shader_ctes_object.lightvec = float4(1.0f, 1.0f, 1.0f, 0.0f);
	shader_ctes_object.lightcol = float4(0.8f, 0.8f, 0.8f, 1.0f);
	shader_ctes_object.ambientcol = float4(0.1f, 0.1f, 0.1f, 1.0f);

	// imgui must be the first to update and the last to render
	auto imgui = new CImGuiModule;
	auto entities = new CEntitiesModule;
	io = new CIOModule;     // It's the global io

	// Will contain all modules created
	all_modules.push_back(imgui);
	all_modules.push_back(entities);
	all_modules.push_back(io);

	mod_update.push_back(imgui);
	
	mod_update.push_back(entities);
	mod_update.push_back(io);

	mod_renders.push_back(entities);
	mod_renders.push_back(imgui);
	mod_renders.push_back(io);
	mod_init_order.push_back(imgui);
	mod_init_order.push_back(entities);
	mod_init_order.push_back(io);
	mod_wnd_proc.push_back(io);
	mod_wnd_proc.push_back(imgui);

	// ----------------------------
	tech_solid_colored = Resources.get("solid_colored.tech")->as<CRenderTechnique>();
	tech_textured_colored = Resources.get("textured.tech")->as<CRenderTechnique>();
	texture1 = Resources.get("textures/wood_d.dds")->as<CTexture>();

	if (!shader_ctes_camera.create("ctes_camera"))
		return false;
	if (!shader_ctes_object.create("ctes_object"))
		return false;

	// Init modules
	for (auto it : mod_init_order) {
		if (!it->start()) {
			dbg("Failed to init module %s\n", it->getName());
			return false;
		}
	}

	io->mouse.toggle();

	return true;
}

// ----------------------------------
void CApp::stop() {
	// Stop modules
	for (auto it = mod_init_order.rbegin(); it != mod_init_order.rend(); ++it)
		(*it)->stop();

	Resources.destroy();
	Debug->destroy();
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

// ----------------------------------
void CApp::update(float elapsed) {
	for (auto it : mod_update)
		it->update(elapsed);

	static float ctime = 0.f;
	ctime += elapsed* 0.01f;

	CHandleManager::destroyAllPendingObjects();
}

// ----------------------------------
void CApp::render() {
	{
		CTraceScoped scope("initFrame");

		// To set a default and known Render State
		Render.ctx->RSSetState(nullptr);

		// Clear the back buffer
		float ClearColor[4] = { 0.3f, 0.3f, 0.3f, 1.0f }; // red,green,blue,alpha
		Render.ctx->ClearRenderTargetView(Render.renderTargetView, ClearColor);
		Render.ctx->ClearDepthStencilView(Render.zbuffer, D3D11_CLEAR_DEPTH, 1.0f, 0);

		tech_solid_colored->activate();

		shader_ctes_object.activate(CTE_SHADER_OBJECT_SLOT);
		shader_ctes_object.World = MAT44::Identity;
		shader_ctes_object.uploadToGPU();

		//Debug TODO Provisional
		Debug->render();

		//Resources.get("grid.mesh")->as<CMesh>()->activateAndRender();
	}
	RenderManager.renderAll();

	for (auto it : mod_renders) {
		CTraceScoped scope(it->getName());
		it->render();
	}
}