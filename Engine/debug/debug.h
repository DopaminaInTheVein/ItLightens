#ifndef INC_DEBUG_H_
#define INC_DEBUG_H_

#include "imgui\imgui.h"

// Some compilers support applying printf-style warnings to user functions.
#if defined(__clang__) || defined(__GNUC__)
#define IM_PRINTFARGS(FMT) __attribute__((format(printf, FMT, (FMT+1))))
#else
#define IM_PRINTFARGS(FMT)
#endif

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))



//standart colors:
#define RED VEC3(1,0,0)
#define GREEN VEC3(0,1,0)
#define BLUE VEC3(0,0,1)

class CDebug {

	struct line {
	public:
		VEC3 org;
		VEC3 end;
		VEC3 color;
	};

	//Console debug attributes:
	//-------------------------------------------------------
	ImGuiTextBuffer     Buf;
	ImGuiTextFilter     Filter;
	ImVector<int>       LineOffsets;        // Index to lines offset
	bool                ScrollToBottom;
	bool opened = false;					//define initial opened console
	//-------------------------------------------------------

	//Debug lines attributes:
	//-------------------------------------------------------
	std::vector<line> lines;
	//-------------------------------------------------------

public:

	CDebug(){}

	void update();
	void render();

	//Console functions:
	//-------------------------------------------------------
	void Clear() { Buf.clear(); LineOffsets.clear(); }
	void LogRaw(const char* msg, ...) IM_PRINTFARGS(2);
	void LogError(const char* msg, ...) IM_PRINTFARGS(2);
	void LogWithTag(const char* tag, const char* msg, ...); IM_PRINTFARGS(3);
	void DrawLog();

	void setOpen(bool newOpened) { opened = newOpened; }
	bool isOpen() { return opened; }
	bool * getStatus() { return &opened; }
	//-------------------------------------------------------

	//Draw lines functions:
	//-------------------------------------------------------
	void DrawLine(VEC3 org, VEC3 end, VEC3 color);
	void DrawLine(VEC3 pos, VEC3 direction, float dist, VEC3 color);
	void RenderLine(line);
	//-------------------------------------------------------

};

extern CDebug *Debug;		//global empty debug declaration --> run initDebugger() to init Debug, we need to initialize ImGui first

							//needed because ImGui need to run first							

/*
* Function to test debug Log
* Random messages every sec
*/
static void TestGameLog()
{
	//TESTING LOG
	//-------------------------------------------------------
	static float last_time = -1.0f;
	float time = ImGui::GetTime();
	if (time - last_time >= 0.3f)
	{
		int rnd = rand() % 3;
		if (rnd == 0) {
			Debug->LogRaw("I am a message without tag\n");
		}
		else if (rnd == 1) {
			int i = 1;
			Debug->LogWithTag("tag - %d", "I am a message with tag+1= %d delta time %f\n", i, i + 1, getDeltaTime());
		}
		else if (rnd == 2) {
			Debug->LogError("I am an ERROR\n");
		}
		else {
			//nothing to do
		}
		last_time = time;
	}
	//-------------------------------------------------------

}



static void testLines() {
	static float var = 0;
	var = fmod(var + getDeltaTime(), 10);
	Debug->DrawLine(VEC3(var,0,0),VEC3(-6,0,1),RED);
	Debug->DrawLine(VEC3(0, var, 0), VEC3(-6, 0, 1), GREEN);
	Debug->DrawLine(VEC3(0, 0, var), VEC3(-6, 0, 1), BLUE);
	Debug->DrawLine(VEC3(var, 0, var), VEC3(-6, 0, 1), VEC3(0.4,0.3,0.6));
}

#endif