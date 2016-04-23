#include "mcv_platform.h"
#include "gui_bar_color.h"
#include "gui_utils.h"

using namespace GUI;

int CGuiBarColor::getBorder()
{
	return border;
}

void CGuiBarColor::setBorder(int new_border)
{
	border = new_border;
}

float CGuiBarColor::getAlpha()
{
	return color.Value.w;
}

void CGuiBarColor::setAlpha(float alpha)
{
	color.Value.w = alpha;
}

void CGuiBarColor::draw(bool borders)
{
	if (borders) drawRect(rectangle, IM_BLACK);
	drawForegroundBar(current);
}

void CGuiBarColor::drawForegroundBar(float fraction) {
	int xmax = rectangle.sx - (border * 2);
	Rect contentBar(
		rectangle.x + border,
		rectangle.y + border,
		xmax * fraction,
		rectangle.sy - (border *2)
		);
	drawRect(contentBar, color);
}