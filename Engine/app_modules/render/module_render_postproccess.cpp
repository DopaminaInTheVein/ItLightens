#include "mcv_platform.h"
#include "module_render_postproccess.h"

bool CRenderPostProcessModule::start()
{

	order_fx.push_back("glow");
	order_fx.push_back("hatching");
	order_fx.push_back("outline");

	//last
	order_fx.push_back("fade_screen");

	return true;
}

void CRenderPostProcessModule::stop()
{
}

void CRenderPostProcessModule::update(float dt)
{
}

void CRenderPostProcessModule::render()
{
	
}
