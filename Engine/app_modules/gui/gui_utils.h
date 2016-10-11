#ifndef INC_MODULE_GUI_UTILS_H_
#define INC_MODULE_GUI_UTILS_H_

struct RectNormalized {
	float x; float y;
	float sx; float sy;
	RectNormalized(float _x, float _y, float _sx, float _sy) :
		x(_x), y(_y), sx(_sx), sy(_sy) {
		assert(_x <= 1.f);
		assert(_y <= 1.f);
		assert(_sx <= 1.f);
		assert(_sy <= 1.f);
	}
	RectNormalized() : x(0), y(0), sx(1.f), sy(1.f) {}

	RectNormalized subRect(RectNormalized sub);
	const RectNormalized operator/=(float v);
	const RectNormalized& operator/(float v);
	const RectNormalized& operator*(float v);
};

struct Rect {
	int x; int y;
	int sx; int sy;
	Rect(int _x, int _y, int _sx, int _sy) :
		x(_x), y(_y), sx(_sx), sy(_sy) {}
	Rect() : x(0), y(0), sx(100), sy(100) {}
	Rect(RectNormalized r);
};

struct Pixel {
	int x;
	int y;
	Pixel() : x(0), y(0) {}
	Pixel(int _x, int _y) : x(_x), y(_y) {}
};

namespace Font {
	struct TCharacter {
	private:
		RectNormalized text_coords;
		float size; //horizontal size grid
		bool space;
		char c;
		std::string special_character;
	public:
		TCharacter() : text_coords(RectNormalized()), size(0.f) {}
		TCharacter(unsigned char c);
		TCharacter(std::string special_char);
		static TCharacter NewLine();
		RectNormalized GetTxtCoords() { return text_coords; }
		float GetSize() { return size; }
		bool IsNewLine() { return c == '\n'; }
		bool IsSpace() { return c == ' '; }
	};

	typedef std::vector<TCharacter> VCharacter;
	RectNormalized getTxtCoords(unsigned char c);
	VCharacter getVChar(std::string text);
	VCharacter formatVChar(VCharacter vchar, float row_size);
}

#endif
