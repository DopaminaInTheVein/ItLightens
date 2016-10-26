#include "mcv_platform.h"
#include "comp_sense_vision.h"

using namespace std;

#include "app_modules/logic_manager/logic_manager.h"

bool TCompSenseVision::load(MKeyValue& atts)
{
	handles = set<CHandle>();
	sense_vision_mode = eSenseVision::DEFAULT;
	return true;
}

void TCompSenseVision::renderInMenu()
{
#ifndef NDEBUG
	static bool sense_vision_bool_default = true;
	static bool sense_vision_bool_enabled = false;
	static bool sense_vision_bool_disabled = false;
	if (ImGui::Checkbox("Default", &sense_vision_bool_default)) {
		if (sense_vision_bool_default) sense_vision_bool_enabled = sense_vision_bool_disabled = false;
		sense_vision_mode = eSenseVision::DEFAULT;
	}
	if (ImGui::Checkbox("Enabled", &sense_vision_bool_enabled)) {
		if (sense_vision_bool_enabled) sense_vision_bool_default = sense_vision_bool_disabled = false;
		sense_vision_mode = eSenseVision::ENABLED;
	}
	if (ImGui::Checkbox("Disabled", &sense_vision_bool_disabled)) {
		if (sense_vision_bool_disabled) sense_vision_bool_enabled = sense_vision_bool_default = false;
		sense_vision_mode = eSenseVision::DISABLED;
	}
#endif // !FINAL_BUILD

	IMGUI_SHOW_BOOL(isSenseVisionEnabled());
}

// Gets
bool TCompSenseVision::isSenseVisionEnabled()
{
	return sense_vision_mode == DEFAULT ? handles.size() > 0 : sense_vision_mode;
}

//Sets
void TCompSenseVision::registerHandle(CHandle h)
{
	if (!isSenseVisionEnabled()) {
		logic_manager->throwEvent(CLogicManagerModule::EVENT::OnSenseVision, CApp::get().getCurrentRealLevel(), h);
	}
	handles.insert(h);
}

void TCompSenseVision::unregisterHandle(CHandle h)
{
	if (handles.size() > 0)
		handles.erase(h);
}

void TCompSenseVision::setSenseVisionMode(bool enabled)
{
	if (enabled) sense_vision_mode = eSenseVision::ENABLED;
	else sense_vision_mode = eSenseVision::DISABLED;
}

void TCompSenseVision::setSenseVisionMode(eSenseVision mode)
{
	sense_vision_mode = mode;
}

void TCompSenseVision::resetVisionMode()
{
	sense_vision_mode = eSenseVision::DEFAULT;
}