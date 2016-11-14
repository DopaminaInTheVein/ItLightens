#include "mcv_platform.h"
#include "module_render_postprocess.h"

#include "comp_basic_fx.h"
#include "test_module_fx.h"

//fx
#include "render\fx\fx_glow.h"
#include "render\fx\fx_fade_screen.h"
#include "render\fx\fx_hatching.h"
#include "render\fx\fx_antialiasing.h"
#include "render\fx\fx_motion_blur.h"
#include "render\fx\fx_outline.h"
#include "render\fx\fx_depth_fog.h"
#include "render\fx\fx_dream.h"
#include "render\fx\fx_ssao.h"

//Initialize fx and add it to the list of fx for the engine
#define INIT_FX( fx_name, fx_object ) \
  fx_object->init(); \
  m_list_fx[fx_name] = fx_object; \

bool CRenderPostProcessModule::start()
{
	//glow
	TRenderGlow* blur = new TRenderGlow;
	INIT_FX(FX_BLUR, blur);

	//fade_screen
	TFadeScreen* fs = new TFadeScreen;
	INIT_FX(FX_FADESCREEN_ALL, fs);
	TFadeScreenAll * fs_all = new TFadeScreenAll;
	INIT_FX("fade_screen_all", fs_all);

	//hatching
	TRenderHatching * hatching = new TRenderHatching;
	INIT_FX(FX_HATCHING, hatching);

	//outline
	TRenderOutline * outline = new TRenderOutline;
	INIT_FX(FX_OUTLINE, outline);

	//dream
	TRenderDream * dream = new TRenderDream;
	INIT_FX(FX_DREAM_BORDER, dream);

	//motion blur
	TRenderMotionBlur * mb = new TRenderMotionBlur;
	INIT_FX(FX_MOTION_BLUR, mb);

	//fog depth
	TRenderDepthFog * df = new TRenderDepthFog;
	INIT_FX(FX_FOG_DEPTH, df);

	//antialiasing
	TRenderAntiAliasing * aa = new TRenderAntiAliasing;
	INIT_FX(FX_ANTI_ALIASING, aa);

	//ssao
	TRenderSSAO * ssao = new TRenderSSAO;
	INIT_FX(FX_SSAO, ssao);

	//###### should go on lua scripts ############

	//NEEDED RENDER SHADERS ON ALL SCENES
	ActivateFXBeforeUI(FX_FOG_DEPTH, 90);
	ActivateFXBeforeUI(FX_OUTLINE, 80);
	ActivateFXBeforeUI(FX_HATCHING, 100);	//default priority is 0, but hatching should go the last one
	ActivateFXBeforeUI(FX_ANTI_ALIASING, 300);

	ActivateFXBeforeUI("dream_border", 150); 

	//test fade
	//WARNING: component fade is the same always, there will be only one!!

	//Init values for fade
	//TFadeScreen* fs = GetFX("fade_screen");
	//fs->FadeIn();

	//<-- from here would start the fade screen effect on render -->
	//ActivateFXBeforeUI(FX_FADESCREEN, 200); //should go as the last one always
	//ActivateFXAtEnd(FX_FADESCREEN_ALL, 200); //should go as the last one always

	//###### end lua scripts #################

	return true;
}

//execute all fx to be executed at the end, managed at deferred module
void CRenderPostProcessModule::ExecuteAllPendentFX()
{
	for (auto& key : m_activated_end) {
		//get handle
		TCompBasicFX* fx = key.fx;

		if (!fx) continue;		//fx not valid
		fx->ApplyFX();
	}
}

//execute all fx before the UI layer, managed at deferred module
void CRenderPostProcessModule::ExecuteUILayerFX()
{
	for (auto& key : m_activated_ui_layer) {
		//get handle
		TCompBasicFX* fx = key.fx;

		if (!fx) continue;		//fx not valid
		fx->ApplyFX();
	}
}

/*
template< typename TObj >
TObj * CRenderPostProcessModule::GetFX(std::string name)
{
	TObj * output = m_list_fx[name];
	return output;
}*/

/*
template< typename TObj >
TObj* CRenderPostProcessModule::GetFX(std::string name)
{
	return dynamic_cast<TObj*>(m_list_fx[name]);
}*/

//higher priority will render the fx above the others with lower priority!!
bool CRenderPostProcessModule::sortByPriority(const TKeyFX &k1, const TKeyFX &k2) {
	return (k1.priority < k2.priority);
}

//Activate a FX default by name
void CRenderPostProcessModule::ActivateFX(std::string name)
{
	TCompBasicFX* handle = m_list_fx[name];
	ActivateFX(handle);
}
//Activate a FX default by handle
void CRenderPostProcessModule::ActivateFX(TCompBasicFX* handle)
{
	handle->Activate();
}
//Activate a FX at the end of the render by name
void CRenderPostProcessModule::ActivateFXAtEnd(std::string name, int priority)
{
	TCompBasicFX* handle = m_list_fx[name];
	ActivateFXAtEnd(handle, priority);
}

//Activate a FX at the end of the render by his handle
void CRenderPostProcessModule::ActivateFXAtEnd(TCompBasicFX* handle, int priority)
{
	static int id = 0;

	TKeyFX new_key = TKeyFX();
	new_key.fx = handle;
	new_key.priority = priority;

	//dbg
	/*dbg("###### INIT #####\n");
	if (!m_activated_end.empty()) {
		for (int id_fx = 0; id_fx < m_activated_end.size(); id_fx++) {
			auto key_fx = m_activated_end[id_fx];
			auto fx = key_fx.fx;
			if (!fx) continue;
			dbg("%d - fx priority %d\n", fx->id, key_fx.priority);
		}
	}*/

	m_activated_end.push_back(new_key);
	std::sort(m_activated_end.begin(), m_activated_end.end(), &sortByPriority);

	/*
	dbg("###### END #####\n");
	if (!m_activated_end.empty()) {
		for (int id_fx = 0; id_fx < m_activated_end.size(); id_fx++) {
			auto key_fx = m_activated_end[id_fx];
			auto fx = key_fx.fx;
			if (!fx) continue;
			dbg("%d - fx priority %d\n", fx->id, key_fx.priority);
		}
	}
	*/
}

void CRenderPostProcessModule::RemoveActiveFX(std::string name)
{
	TCompBasicFX* handle = m_list_fx[name];
	RemoveActiveFX(handle);
}

void CRenderPostProcessModule::RemoveActiveFX(TCompBasicFX* handle)
{
	for (auto fx_key = m_activated_ui_layer.begin(); fx_key != m_activated_ui_layer.end();) {
		if (fx_key->fx == handle) {
			fx_key = m_activated_ui_layer.erase(fx_key);
		}
		else fx_key++;
	}

	for (auto fx_key = m_activated_end.begin(); fx_key != m_activated_end.end();) {
		if (fx_key->fx == handle) {
			fx_key = m_activated_end.erase(fx_key);
		}
		else fx_key++;
	}
}

//Activate a FX before the UI layer is run by name.
void CRenderPostProcessModule::ActivateFXBeforeUI(std::string name, int priority)
{
	TCompBasicFX* handle = m_list_fx[name];
	ActivateFXBeforeUI(handle, priority);
}

//Activate a FX before the UI layer is run by his handle
void CRenderPostProcessModule::ActivateFXBeforeUI(TCompBasicFX* handle, int priority)
{
	static int id = 0;

	TKeyFX new_key = TKeyFX();
	new_key.fx = handle;
	new_key.priority = priority;

	//dbg
	/*dbg("###### INIT #####\n");
	if (!m_activated_ui_layer.empty()) {
		for (int id_fx = 0; id_fx < m_activated_ui_layer.size(); id_fx++) {
			auto key_fx = m_activated_ui_layer[id_fx];
			auto fx = key_fx.fx;
			if (!fx) continue;
			dbg("%d - fx priority %d\n", fx->id, key_fx.priority);
		}
	}*/

	m_activated_ui_layer.push_back(new_key);
	std::sort(m_activated_ui_layer.begin(), m_activated_ui_layer.end(), &sortByPriority);

	/*dbg("###### END #####\n");
	if (!m_activated_ui_layer.empty()) {
		for (int id_fx = 0; id_fx < m_activated_ui_layer.size(); id_fx++) {
			auto key_fx = m_activated_ui_layer[id_fx];
			auto fx = key_fx.fx;
			if (!fx) continue;
			dbg("%d - fx priority %d\n", fx->id, key_fx.priority);
		}
	}*/
}

//Add a FX to the list of fx for the engine
void CRenderPostProcessModule::AddFX(std::string name, TCompBasicFX* handle)
{
	handle->init();
	m_list_fx[name] = handle;
}

bool CRenderPostProcessModule::isActive(std::string name)
{
	int idx = 0;
	for (auto fx_key : m_activated_ui_layer) {
		if (fx_key.fx == m_list_fx[name]) {
			return true;
		}
		idx++;
	}

	idx = 0;
	for (auto fx_key : m_activated_end) {
		if (fx_key.fx == m_list_fx[name]) {
			return true;
		}
		idx++;
	}
	return false;
}

TCompBasicFX* CRenderPostProcessModule::GetBasicFX(std::string name)
{
	int idx = 0;
	for (auto fx_key : m_activated_ui_layer) {
		if (fx_key.fx == m_list_fx[name]) {
			return m_list_fx[name];
		}
		idx++;
	}

	idx = 0;
	for (auto fx_key : m_activated_end) {
		if (fx_key.fx == m_list_fx[name]) {
			return m_list_fx[name];
		}
		idx++;
	}
	return nullptr;
}

//clear all memory
void CRenderPostProcessModule::stop()
{
	m_activated_end.clear();
	m_activated_ui_layer.clear();

	//clear all memory from fx
	for (auto& pair : m_list_fx) {
		delete pair.second;
	}
	m_list_fx.clear();
}

void CRenderPostProcessModule::update(float dt)
{
	for (auto& pair : m_list_fx) {
		pair.second->update(dt);
	}
}

void CRenderPostProcessModule::renderInMenu()
{
	if (ImGui::TreeNode("all postprocess")) {
		for (auto& pair : m_list_fx) {
			if (ImGui::TreeNode(pair.second->getName())) {
				pair.second->renderInMenu();
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Activated FX before UI")) {
		for (int id_fx = 0; id_fx < m_activated_ui_layer.size(); id_fx++) {
			auto key_fx = m_activated_ui_layer[id_fx];
			auto fx = key_fx.fx;
			if (!fx) continue;
			if (ImGui::TreeNode(fx->getName())) {
				fx->renderInMenu();
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Activated FX at the end")) {
		for (int id_fx = 0; id_fx < m_activated_end.size(); id_fx++) {
			auto key_fx = m_activated_end[id_fx];
			auto fx = key_fx.fx;
			if (!fx) continue;
			if (ImGui::TreeNode(fx->getName())) {
				fx->renderInMenu();
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
}