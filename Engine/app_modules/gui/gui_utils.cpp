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
	this->c = c;
}
TCharacter Font::TCharacter::NewLine()
{
	TCharacter res = TCharacter();
	res.c = '\n';
	res.special_character = "\n";
	return res;
}
Font::TCharacter::TCharacter(std::string special_char)
{
	*this = Gui->getSpecialChar(special_char);
}

Font::TCharacter::TCharacter(std::string name, int row, int col, float size)
{
	special_character = name;
	text_coords = Font::getTxtCoords(row * 16 + col);
	text_coords.sx *= ceil(size);
	this->size = size;
	c = '*';
}

class Parser {
private:
	//Input
	std::string input;

	//Output
	VCharacter result;

	//Intern
	int index = 0;
	char color[10] = "#FFFFFFFF";
	char value = 0;
	bool RemainChars() {
		return index < input.length();
	}
	bool NextChar() {
		if (index >= input.length()) return false;
		value = input[index++];
	}
	bool IsChar(char c) {
		return c == value;
	}

	void WriteChar(TCharacter tchar)
	{
		tchar.SetColor(color);
		result.push_back(tchar);
	}
public:
	Parser(std::string s) {
		input = s;
		parse();
	}

	VCharacter getVChars() { return result; }

	void parse()
	{
		while (RemainChars()) {
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
			else if (IsChar('#')) {
				sprintf(color, "#FFFFFFFF");
				int i = 1;
				NextChar();
				while (i < 10 && !IsChar('#')) {
					color[i++] = value;
					NextChar();
				}
			}

			//Special char
			else if (IsChar('*')) {
				char special_char[128];
				int i_special = 0;
				NextChar();
				while (!IsChar('*')) {
					special_char[i_special++] = value;
					NextChar();
				}
				special_char[i_special] = 0;
				WriteChar(TCharacter(special_char));
			}

			//Regular char
			else {
				WriteChar(TCharacter(value));
			}
		}
	}
};

VCharacter Font::getVChar(std::string text)
{
	Parser parser(text);
	return parser.getVChars();
}

class Formatter {
private:
	//Input
	VCharacter input;
	float max_row_size;

	//Intern
	VCharacter buffer;
	VCharacter output;
	int writed_buffer = 0;
	int writed_result = 0;
	float cur_line_size = 0;
	float buffer_line_size = 0;
	bool skipped_line = false;

	void append_result(TCharacter tchar) {
		output[writed_result++] = tchar;
		if (tchar.IsNewLine()) {
			cur_line_size = 0;
		}
		else cur_line_size += tchar.GetSize();
	}

	void buffer_to_result() {
		if (writed_buffer == 0) return; // Just in case

		//Check enough space in this line, otherwise move to next line
		if (cur_line_size + buffer_line_size > max_row_size) {
			append_result(TCharacter::NewLine());
			cur_line_size = 0;
			skipped_line = true;
		}

		int next = 0;
		//If beginning line, ignore spaces
		if (cur_line_size == 0) {
			while (next < writed_buffer && (buffer[next].IsSpace() || buffer[next].IsNewLine())) next++;
		}
		while (next < writed_buffer) {
			if (skipped_line && buffer[next].IsNewLine()) skipped_line = false;
			else append_result(buffer[next++]);
		}
		writed_buffer = buffer_line_size = 0;
	}

	void append_buffer(TCharacter tchar) {
		buffer[writed_buffer++] = tchar;
		buffer_line_size += tchar.GetSize();
	}

public:
	Formatter(VCharacter i, float rs) : input(i), max_row_size(rs) {}
	VCharacter format() {
		output = VCharacter(input.size() * 2);
		buffer = VCharacter(input.size() * 2);
		for (auto tchar : input) {
			if (tchar.IsSpace() || tchar.IsNewLine()) {
				buffer_to_result();
			}
			if (skipped_line && tchar.IsNewLine()) { //If we skip a line cause run out of space, dont write next endline
				skipped_line = false;
			}
			else {
				append_buffer(tchar);
			}
		}
		buffer_to_result();
		output.resize(writed_result);
		return output;
	}
};

VCharacter Font::formatVChar(VCharacter vchar, float row_size)
{
	Formatter formatter = Formatter(vchar, row_size);
	return formatter.format();
}