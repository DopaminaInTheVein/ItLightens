#include "mcv_platform.h"
#include "gui_cursor.h"

#include "components/entity.h"
#include "components/comp_transform.h"

#include "app_modules/gameController.h"
#include "app_modules/io/io.h"

// load Xml
bool TCompGuiCursor::load(MKeyValue& atts)
{
	GameController->SetUiControl(true);
	speed = atts.getFloat("speed", 5.f);
	return true;
}

//void TCompGuiCursor::onCreate(const TMsgEntityCreated&)
//{
//	GameController->SetUiControl(true);
//}

bool TCompGuiCursor::getUpdateInfo()
{
	myTransform = GETH_MY(TCompTransform);
	if (!myTransform) return false;

	return true;
}

void TCompGuiCursor::update(float dt)
{
	//Leer mouse
	float dx = 0, dy = 0;
	dx += io->mouse.dx;
	dy += io->mouse.dy;

	//Apply speed
	float factor = speed * getDeltaTime();
	dx *= factor;
	dy *= factor;

	//Apply Movement
	VEC3 movement = VEC3(dx, -dy, 0);
	myTransform->addPosition(movement);
}

void TCompGuiCursor::renderInMenu()
{
}

TCompGuiCursor::~TCompGuiCursor()
{
	GameController->SetUiControl(false);
}