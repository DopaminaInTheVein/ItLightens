#include "mcv_platform.h"
#include "module_imgui.h"
#include "windows/app.h"
#include "resources/resources_manager.h"
#include "app_modules/entities.h"
#include "app_modules/logic_manager/logic_manager.h"
#include "app_modules/gui/gui.h"
#include "handle/handle_manager.h"
#include "components/entity_tags.h"
#include "components/components.h"
// ImGui LIB headers
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui.h"
#pragma comment(lib, "imgui.lib" )

#include "handle/object_manager.h"
#include "render/draw_utils.h"

#include "particles\particles_manager.h"

#include "app_modules\render\module_render_postprocess.h"
#include "app_modules\lang_manager\lang_manager.h"

//editors
#include "Editors\editor_lights.h"
#include "Editors\editor_messages.h"

#include <Commdlg.h>

#ifdef CALIBRATE_GAME
#include "components/components.h"
#endif

//light editor
void CImGuiModule::StartLightEditor() {
	m_pLights_editor = new CEditorLights;
	m_pLights_editor->LoadLights();
}
//messages editor
void CImGuiModule::StartMessagesEditor() {
	m_pMessages_editor = new CEditorMessages;
	m_pMessages_editor->LoadTexts();
}

ImGuiTextFilter CImGuiModule::filter = ImGuiTextFilter();

bool CImGuiModule::start() {
	CApp& app = CApp::get();
	return ImGui_ImplDX11_Init(app.getHWnd(), Render.device, Render.ctx);
}

void CImGuiModule::stop() {
	delete m_pLights_editor;
	delete m_pMessages_editor;
	ImGui_ImplDX11_Shutdown();
}

void CImGuiModule::update(float dt) {
	ImGui_ImplDX11_NewFrame();

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_MenuBar;
	bool menu = true;
#ifndef FINAL_BUILD
	if (controller->IsToogleDebugUIPressed()) show_imgui = !show_imgui;
	if (!show_imgui) return;
	ImGui::Begin("Debug UI", &menu, ImVec2(800, 512), -1.0f, window_flags);
	ImGui::PushItemWidth(-140);                                 // Right align, keep 140 pixels for labels

	m_pLights_editor->update(dt);
	m_pMessages_editor->update(dt);

	//Engine Apps
	//---------------------------------------
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Apps"))
		{
			ImGui::MenuItem("Log (L)", NULL, Debug->getStatus());
			ImGui::MenuItem("Commands (O)", NULL, Debug->GetCommandsConsoleState());

			ImGui::MenuItem("Particle editor (F8)", NULL, g_particlesManager->GetParticleEditorState());
			ImGui::MenuItem("Lights editor (F9)", NULL, m_pLights_editor->GetLightsEditorState());
			ImGui::MenuItem("Messages editor (F10)", NULL, m_pMessages_editor->GetEditorState());

			//Debug->OpenConsole();
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	//---------------------------------------
	//Current Level
	IMGUI_SHOW_INT(CApp::get().GetLevelNumber());

	//Language
	IMGUI_SHOW_STRING(lang_manager->GetLanguage());

	//Difficulty
	IMGUI_SHOW_INT(GameController->GetDifficulty());

	//Extra wait end frame
	if (ImGui::TreeNode("Physx Timing")) {
		float tmax_update = g_PhysxManager->GetTMaxUpdate();
		ImGui::PushItemWidth(100);
		if (ImGui::DragFloat("tmax_update", &tmax_update, 0.01f)) {
			g_PhysxManager->SetTMaxUpdate(tmax_update);
		}
		ImGui::DragFloat("Wait end frame", CApp::get().GetWaitEnd());
		if (ImGui::Checkbox("Multi simulate", g_PhysxManager->GetMultiSimulatePointer())) {
			dbg("Changed Multisimulate\n");
		}
		ImGui::PopItemWidth();
		ImGui::TreePop();
	}

	//Last Input
	ImGui::Checkbox("Gamepad Mode", io->IsGamePadModePointer());

	//Buttons game
	//---------------------------------------
	if (GameController->GetGameState() == CGameController::RUNNING) {
		IMGUI_SHOW_INT(CGameController::RUNNING);
		if (ImGui::Button("PAUSE BUTTON"))
			GameController->SetGameState(CGameController::STOPPED);

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 1, 0, 1));
		if (ImGui::Button("RESUME BUTTON"))
			GameController->SetGameState(CGameController::RUNNING);

		ImGui::PopStyleColor();
	}
	else if (GameController->GetGameState() == CGameController::STOPPED) {
		IMGUI_SHOW_INT(CGameController::STOPPED);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 1, 0, 1));
		if (ImGui::Button("PAUSE BUTTON"))
			GameController->SetGameState(CGameController::STOPPED);

		ImGui::PopStyleColor();

		ImGui::SameLine();

		if (ImGui::Button("RESUME BUTTON"))
			GameController->SetGameState(CGameController::RUNNING);
	}
	ImGui::SameLine();
	if (ImGui::Button("SKIP Cine.")) {
		logic_manager->throwUserEvent("cam:skip_cinematic();");
	}
	//Select Level
	{
		ImGui::PushItemWidth(80);
		static int level_selected = 1;
		if (ImGui::InputInt("", &level_selected)) {
			if (level_selected > 4) level_selected = 0;
			if (level_selected < 0) level_selected = 4;
		}
		ImGui::SameLine();
		auto name_scenes = CApp::get().GetNameScenes();
		char logic_level[] = "level_X";
		logic_level[6] = '0' + (unsigned char)(level_selected);
		ImGui::SameLine();
		ImGui::Text("%s", name_scenes[logic_level].c_str());
		ImGui::SameLine();
		if (ImGui::Button("Load Level")) {
			char lua_code[256];
			sprintf(lua_code, "p:stop_music(); p:setup_game(); LoadLevel(\"level_%d\")", level_selected);
			logic_manager->throwUserEvent(lua_code);
		}
		ImGui::PopItemWidth();
	}

	if (ImGui::Button("SAVE GAME")) CApp::get().saveLevel();
	if (ImGui::Button("LOAD GAME")) CApp::get().restartLevelNotify();

	ImGui::Checkbox("Free camera (K)", GameController->GetFreeCameraPointer());
	ImGui::Checkbox("Ui control", Gui->IsUiControlPointer());
	//ImGui::Checkbox("Continous Collision Detection", &(g_PhysxManager->ccdActive));

	if (ImGui::TreeNode("LUA Test")) {
		static char test_lua_txt[256] = "OnTest();";
		ImGui::InputTextMultiline("Test Lua", test_lua_txt, 256);
		if (ImGui::Button("Execute")) {
			logic_manager->throwUserEvent(test_lua_txt);
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Gui create elements")) {
		static VEC3 pos_new_ui = VEC3(0.5f, 0.5f, 0.9f);
		static char gui_prebab_name[64] = "Fading_Letter";
		static char gui_prebab_entity_name[64] = "TEST";

		ImGui::DragFloat3("Pos", &pos_new_ui.x, 0.1f, -1.f, 2.f);
		ImGui::InputText("Prefab name", gui_prebab_name, 64);
		ImGui::InputText("Entity name", gui_prebab_entity_name, 64);
		if (ImGui::Button("Create")) {
			CHandle gui_elem = Gui->addGuiElement(std::string("ui/") + std::string(gui_prebab_name), pos_new_ui);
			GET_COMP(name, gui_elem, TCompName);
			name->setName(gui_prebab_entity_name);
		}

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("TestMessages")) {
		static char test_msg_txt[256] = "*MOUSE* *LMOUSE* *RMOUSE*\n*LB* *RB* *BACK* *START* *LSTICK* *RSTICK* *APAD* *BPAD* *XPAD* *YPAD*\n*SHIFT* *ENTER* *SPACE* *ESC* *WKEY* *AKEY* *SKEY* *DKEY*";
		ImGui::InputTextMultiline("Test Message", test_msg_txt, 256);
		if (ImGui::Button("Create Message")) {
			std::string text = test_msg_txt;
			getHandleManager<TCompFadingMessage>()->each([text](TCompFadingMessage * mess) {
				TCompFadingMessage::ReloadInfo atts;
				atts.text = text;
				mess->reload(atts);
			}
			);
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Free camera instructions")) {
		ImGui::Text("w/a/s/d - normal move\nq/e - up/down\nmouse wheel - speed up/down\n");

		ImGui::TreePop();
	}

	ImGui::Separator();
	//---------------------------------------
#ifdef CALIBRATE_GAME
	if (ImGui::TreeNode("CALIBRATE")) {
		if (ImGui::TreeNode("Bomb sci")) {
			ImGui::Checkbox("Calibrate", &CThrowBomb::calibrate);
			IMGUI_DRAG_FLOAT(CThrowBomb::lmax_st, 0.01f, 0.1f, 10.f);
			IMGUI_DRAG_FLOAT(CThrowBomb::hmax_st, 0.01f, 0.1f, 10.f);
			IMGUI_DRAG_FLOAT(CThrowBomb::speed_st, 0.01f, 0.1f, 10.f);
			IMGUI_DRAG_FLOAT(CThrowBomb::radius_st, 0.01f, 0.1f, 10.f);
			ImGui::DragFloat3("offset start throw", &CThrowBomb::offset_init_throw.x, 0.01f, -1.f, 1.f);
			ImGui::TreePop();
}
		ImGui::TreePop();
}
#endif
	//Profiling
	//---------------------------------------
#ifdef PROFILING_ENABLED
  //header for filtering instructions
	if (ImGui::TreeNode("PROFILING"))
	{
		static int nframes = 5;
		static float time_threshold = 5;
		ImGui::InputInt("NFrames", &nframes, 1, 5);
		ImGui::Separator();
		if (ImGui::Button("Start Capture Profiling"))
			profiler->setNFramesToCapture(nframes);
#ifndef PROFILING_JOHN
		ImGui::Separator();
		ImGui::InputFloat("Time Threshold", &time_threshold, 1, 5);
		if (profiler->isAutoCapture()) {
			ImGui::Text("Waiting auto capture...");
		}
		else {
			if (ImGui::Button("Auto Capture Profiling"))
				profiler->setAutoCapture(nframes, time_threshold);
		}
#endif
		ImGui::TreePop();
	}
#endif
	//---------------------------------------

	//Filter options
	//---------------------------------------
	ImGui::Text("Filtering");

	//header for filtering instructions
	if (ImGui::TreeNode("instructions filtering"))
	{
		ImGui::Text("Filter usage:\n"
			"  \"\"         display all lines\n"
			"  \"xxx\"      display lines containing \"xxx\"\n"
			"  \"xxx,yyy\"  display lines containing \"xxx\" or \"yyy\"\n"
			"  \"-xxx\"     hide lines containing \"xxx\"");
		ImGui::TreePop();
	}
	//end header instructions

	//static ImGuiTextFilter filter;
	filter.Draw(); //filter text draw
	//---------------------------------------

	if (ImGui::CollapsingHeader("General Info")) {
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
	if (ImGui::CollapsingHeader("Resources")) {
		Resources.renderUIDebug(&filter);
	}

	if (ImGui::CollapsingHeader("Entities")) {
		getHandleManager<CEntity>()->onAll(&CEntity::renderInMenu);
	}

	if (ImGui::CollapsingHeader("PostProcess")) {
		render_fx->renderInMenu();
	}

	if (ImGui::CollapsingHeader("SELECTED ENTITY")) {
		ImGui::Text("Application SELECTED ENTITY - TODO");
	}if (ImGui::CollapsingHeader("Entity by Tag")) {
		ImGui::Text("Application ENTITY TAG - TODO");
		tags_manager.renderInMenu();
	}
	if (ImGui::CollapsingHeader("Game Options")) {
		ImGui::Checkbox("god mode", GameController->GetCheatGodmodePointer());
	}if (ImGui::CollapsingHeader("Graficos")) {
		if (ImGui::TreeNode("polarize")) {
			//ImGui::SliderFloat("Polarize strength", &shader_ctes_globals.strenght_polarize, 0.0f, 2.0f);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Dream shader options")) {
			if (ImGui::DragFloat4("color influence", &shader_ctes_dream.color_influence.x, 0.01f)) {
				shader_ctes_dream.uploadToGPU();
			}
			if (ImGui::DragFloat("waves size", &shader_ctes_dream.dream_waves_size, 0.01f)) {
				shader_ctes_dream.uploadToGPU();
			}
			if (ImGui::DragFloat("waves speed", &shader_ctes_dream.dream_speed, 0.01f)) {
				shader_ctes_dream.uploadToGPU();
			}
			if (ImGui::DragFloat("waves intensity", &shader_ctes_dream.dream_wave_amplitude, 0.01f)) {
				shader_ctes_dream.uploadToGPU();
			}
			if (ImGui::DragFloat("distorsion strenght", &shader_ctes_dream.dream_distorsion_strenght, 0.01f)) {
				shader_ctes_dream.uploadToGPU();
			}
			if (ImGui::DragFloat("distorsion expansion", &shader_ctes_dream.dream_distorsion_expansion, 0.01f)) {
				shader_ctes_dream.uploadToGPU();
			}

			if (ImGui::SmallButton("Load defaults")) {
				shader_ctes_dream.color_influence = VEC4(0.5, 0.5, 0.8, 1);
				shader_ctes_dream.dream_speed = 1;
				shader_ctes_dream.dream_waves_size = 0.5;
				shader_ctes_dream.dream_wave_amplitude = 8;

				shader_ctes_dream.dream_distorsion_expansion = 0.85;
				shader_ctes_dream.dream_distorsion_strenght = 55;
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Shadow options")) {
			if (ImGui::DragFloat("Environment diffuse factor", &shader_ctes_globals.env_factor, 0.01f)) {
				shader_ctes_globals.uploadToGPU();
			}

			if (ImGui::DragFloat("Shadow intensity", &shader_ctes_globals.shadow_intensity, 0.01f)) {
				shader_ctes_globals.uploadToGPU();
			}

			if (ImGui::TreeNode("ssao")) {
				if (ImGui::DragFloat("ssao intensity", &shader_ctes_blur.ssao_intensity)) {
					shader_ctes_blur.uploadToGPU();
				}

				if (ImGui::DragFloat("ssao iterations", &shader_ctes_blur.ssao_iterations)) {
					shader_ctes_blur.uploadToGPU();
				}

				ImGui::Separator();

				if (ImGui::DragFloat("ssao intensity test", &shader_ctes_blur.ssao_test_intensity, 0.01f)) {
					shader_ctes_blur.uploadToGPU();
				}

				if (ImGui::DragFloat("ssao bias test", &shader_ctes_blur.ssao_bias, 0.01f)) {
					shader_ctes_blur.uploadToGPU();
				}

				if (ImGui::DragFloat("ssao scale test", &shader_ctes_blur.ssao_scale, 0.01f)) {
					shader_ctes_blur.uploadToGPU();
				}

				if (ImGui::DragFloat("ssao rad test", &shader_ctes_blur.ssao_sample_rad, 0.01f)) {
					shader_ctes_blur.uploadToGPU();
				}

				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Grafic options")) {
			ImGui::Checkbox("polarize effects(disabled)", GameController->GetFxPolarizePointer());

			ImGui::Checkbox("glow effect(disabled)", GameController->GetFxGlowPointer());

			if (ImGui::DragFloat("Specular force", &shader_ctes_hatching.specular_force)) {
				shader_ctes_hatching.uploadToGPU();
			}

			static bool use_ramp = shader_ctes_globals.use_ramp;
			if (ImGui::Checkbox("use ramp", &use_ramp)) {
				shader_ctes_globals.use_ramp = use_ramp;
			}

			static bool use_ramp_color = shader_ctes_globals.use_ramp_color;
			if (ImGui::Checkbox("use ramp color", &use_ramp_color)) {
				shader_ctes_globals.use_ramp_color = use_ramp_color;
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Hatching test")) {
			ImGui::Text("Limit lines hatching (DISABLED, not working)");
			if (ImGui::DragFloat("Rim strength", &shader_ctes_hatching.rim_strenght, 0.1f)) {
				shader_ctes_hatching.uploadToGPU();
			}
			if (ImGui::DragFloat("Specular strength", &shader_ctes_hatching.specular_strenght, 0.1f)) {
				shader_ctes_hatching.uploadToGPU();
			}
			if (ImGui::DragFloat("Diffuse strength", &shader_ctes_hatching.diffuse_strenght, 0.1f)) {
				shader_ctes_hatching.uploadToGPU();
			}

			ImGui::Separator();
			ImGui::Text("Lines hatching options");
			if (ImGui::DragFloat("Size hatching lines", &shader_ctes_hatching.frequency_texture, 0.1f)) {
				shader_ctes_hatching.uploadToGPU();
			}
			if (ImGui::DragFloat("Intensity transparency lines", &shader_ctes_hatching.intensity_sketch, 0.1f)) {
				shader_ctes_hatching.uploadToGPU();
			}
			if (ImGui::DragFloat("Frequency change offset", &shader_ctes_hatching.frequency_offset, 0.1f)) {
				shader_ctes_hatching.uploadToGPU();
			}

			ImGui::Separator();
			ImGui::Text("Outline options");
			ImGui::Text("Quantity lines: more low to more lines. Will do strange things");
			if (ImGui::DragFloat("Quantity lines found", &shader_ctes_hatching.edge_lines_detection, 0.0001f, 0.0000000001f)) {
				shader_ctes_hatching.uploadToGPU();
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Ramp shading options")) {
			ImGui::Text("Color influence from ramp, will work better with white lights, will mix lights");

			bool color_ramp = false;

			if (shader_ctes_hatching.color_ramp == 0.0f)
				color_ramp = false;
			else
				color_ramp = true;

			if (ImGui::Checkbox("color from ramp", &color_ramp)) {
				if (color_ramp) {
					shader_ctes_hatching.color_ramp = 1.0f;
					shader_ctes_hatching.uploadToGPU();
				}
				else {
					shader_ctes_hatching.color_ramp = 0.0f;
					shader_ctes_hatching.uploadToGPU();
				}
			}

			ImGui::TreePop();
		}
	}if (ImGui::CollapsingHeader("Culling")) {
		RenderManager.renderUICulling(SBB::readSala());
		TCompSkeleton::renderUICulling();
		ImGui::Checkbox("show culling collider", GameController->GetCullingRenderPointer());
	}

	if (ImGui::CollapsingHeader("Particles")) {
		g_particlesManager->renderInMenu();
	}

	ImGui::End();
#endif
	//TESTS DEBUG:
	//TestGameLog();
	//testLines();
	//bool open = true;
	//ShowExampleAppConsole(&open);		//test console commands

	//Debug->update();		//update log
	m_pLights_editor->RenderInMenu();
	m_pMessages_editor->RenderInMenu();
}

void CImGuiModule::render() {
	activateZ(ZCFG_ALL_DISABLED);
	ImGui::Render();
}

bool CImGuiModule::onSysMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	return ImGui_ImplDX11_WndProcHandler(hWnd, message, wParam, lParam) ? true : false;
}

//Open file path
std::string CImGuiModule::getFilePath(char * filter, HWND owner)
{
	OPENFILENAME ofn;
	char fileName[MAX_PATH] = "";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = owner;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	ofn.lpstrDefExt = "";
	std::string fileNameStr;
	if (GetOpenFileName(&ofn))
		fileNameStr = fileName;

	return fileNameStr;
}

// Text Filter
std::string CImGuiModule::getFilterText()
{
	return std::string(filter.InputBuf);
}