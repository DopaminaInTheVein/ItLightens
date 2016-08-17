#include "mcv_platform.h"
#include "gui_button.h"

#include "components/entity.h"
#include "components/comp_transform.h"

#include "app_modules/gameController.h"
#include "app_modules/io/io.h"
#include "app_modules/logic_manager/logic_manager.h"

// load Xml
bool TCompGuiButton::load(MKeyValue& atts)
{
	width = atts.getFloat("width", 0.f);
	height = atts.getFloat("height", 0.f);

	return true;
}

//void TCompGuiCursor::onCreate(const TMsgEntityCreated&)
//{
//	GameController->SetUiControl(true);
//}

bool TCompGuiButton::getUpdateInfo()
{
	myTransform = GETH_MY(TCompTransform);
	if (!myTransform) return false;

	cursor = tags_manager.getFirstHavingTag("gui_cursor");
	if (!cursor.isValid()) return false;

	cursorTransform = GETH_COMP(cursor, TCompTransform);
	if (!cursorTransform) return false;

	return true;
}

void TCompGuiButton::update(float dt)
{
	VEC3 myPos = myTransform->getPosition();
	VEC3 cursorPos = cursorTransform->getPosition();
	VEC3 delta = myPos - cursorPos;
	bool is_over_now = abs(delta.x) < width*0.5f && abs(delta.y) < height*0.5f;
	dbg("\n\nButton pos: %f, %f. Cursor pos: %f, %f. width: %f, height: %f\n%d\n\n\n\n", myPos.x, myPos.y, cursorPos.x, cursorPos.y, width, height, is_over_now);
	if (is_over_now ^ cursor_over) {
		//Send message
		TMsgOverButton msg;
		msg.button = CHandle(this).getOwner();
		msg.is_over = is_over_now;
		cursor.sendMsg(msg);
	}
	cursor_over = is_over_now;
}

void TCompGuiButton::onClick(const TMsgClicked&)
{
	logic_manager->throwEvent(CLogicManagerModule::EVENT::OnClicked, MY_NAME, MY_OWNER);
}

void TCompGuiButton::renderInMenu()
{
}