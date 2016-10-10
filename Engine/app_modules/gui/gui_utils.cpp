#include "mcv_platform.h"
#include "gui_utils.h"

#include "gui.h"

RectNormalized RectNormalized::subRect(RectNormalized sub)
{
	RectNormalized res;
	res.x = x + sub.x;
	res.y = y + sub.y;
	res.sx = sx * sub.sx;
	res.sy = sy * sub.sy;
	return res;
}
const RectNormalized RectNormalized::operator/=(float v) {
	x /= v;
	y /= v;
	sx /= v;
	sy /= v;
	return *this;
}
const RectNormalized& RectNormalized::operator/(float v) {
	RectNormalized res;
	res.x = x / v;
	res.y = y / v;
	res.sx = sx / v;
	res.sy = sy / v;
	return res;
}
const RectNormalized& RectNormalized::operator*(float v) {
	RectNormalized res;
	res.x = x * v;
	res.y = y * v;
	res.sx = sx * v;
	res.sy = sy * v;
	return res;
}

Rect::Rect(RectNormalized r) {
	float res_x = CApp::get().getXRes();
	float res_y = CApp::get().getYRes();
	x = r.x * res_x;
	y = r.y * res_y;
	sx = r.sx * res_x;
	sy = r.sy * res_y;
}

using namespace Font;
#define LETTER_BOX_SIZE (1.f / 16.f)

RectNormalized Font::getTxtCoords(unsigned char c)
{
	int ascii_tex_posx = c % 16;
	int ascii_tex_posy = c / 16;

	float texture_pos_x = ((float)ascii_tex_posx) * LETTER_BOX_SIZE;
	float texture_pos_y = ((float)ascii_tex_posy) * LETTER_BOX_SIZE;
	float sx = LETTER_BOX_SIZE;
	float sy = LETTER_BOX_SIZE;

	return RectNormalized(texture_pos_x, texture_pos_y, sx, sy);
}

Font::TCharacter::TCharacter(unsigned char c)
{
	text_coords = getTxtCoords(c);
	size = Gui->getCharSize(c);
	new_line = false;
#ifndef NDEBUG
	this->c = c;
#endif
}
TCharacter Font::TCharacter::NewLine()
{
	TCharacter res = TCharacter();
	res.new_line = true;
#ifndef NDEBUG
	res.special_character = "\n";
#endif
	return res;
}
Font::TCharacter::TCharacter(std::string special_char)
{
#ifndef NDEBUG
	special_character = special_char;
#endif
	dbg("Aqui deberia ir el special char\n");
}

#define InputChar input_char
#define ThisChar current_char
#define ResultChar res
#define IndexChar index_char
#define RemainsChar() (IndexChar < InputChar.length())
#define IsChar(c) (ThisChar == c)
#define NextChar() if (IndexChar >= InputChar.length()) {break;} else ThisChar = InputChar[IndexChar++]
#define WriteChar(c) ResultChar.push_back(c);
VCharacter Font::getVChar(std::string InputChar)
{
	VCharacter ResultChar = VCharacter();
	int IndexChar = 0;
	char ThisChar = 0;
	while (RemainsChar()) {
		NextChar();
		// Next line
		if (IsChar('\n')) {
			WriteChar(TCharacter::NewLine());
		}
		if (IsChar('\\')) {
			NextChar();
			if (IsChar('n')) {
				WriteChar(TCharacter::NewLine());
			}
			else {
				assert(fatal("Error parsing Vcharacter (/?)\n"));
			}
		}

		//Special char
		else if IsChar('*') {
			NextChar();
			char special_char[128];
			int i_special = 0;
			while (!IsChar('*')) {
				special_char[i_special++] = ThisChar;
				NextChar();
			}
			special_char[i_special] = 0;
			WriteChar(TCharacter(special_char));
		}

		//Regular char
		else {
			WriteChar(TCharacter(ThisChar));
		}
	}
	return ResultChar;
}