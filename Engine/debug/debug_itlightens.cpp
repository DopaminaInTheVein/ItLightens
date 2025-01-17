#include "mcv_platform.h"
#include "debug_itlightens.h"
#include "render/mesh.h"
#include "resources/resource.h"
#include "render/shader_cte.h"
#include "constants/ctes_object.h"
#include "resources\resources_manager.h"

extern CShaderCte< TCteObject > shader_ctes_object;

struct SimpleVertexColored {
	float x, y, z;
	float r, g, b, a;
	void set(VEC3 npos, VEC4 color) {
		x = npos.x;
		y = npos.y;
		z = npos.z;
		r = color.x;
		g = color.y;
		b = color.z;
		a = color.w;
	}
};

void  CDebug::LogRaw(const char* msg, ...)
{
#ifndef NDEBUG					//performance cost
	int old_size = Buf.size();

	//near max int
	if (old_size >= 2047483647) {
		Clear();
		old_size = Buf.size();
	}
	va_list args;
	va_start(args, msg);
	Buf.appendv(msg, args);
	va_end(args);
	for (int new_size = Buf.size(); old_size < new_size; old_size++)
		if (Buf[old_size] == '\n')
			LineOffsets.push_back(old_size);
	ScrollToBottom = true;

	//visual studio log
	dbg(Buf.c_str());
#else
	(void)(true);
#endif
}

void  CDebug::LogError(const char* msg, ...)
{
	LogWithTag("Error", msg);
}

void CDebug::LogWithTag(const char * tag, const char * msg, ...)
{
#ifndef NDEBUG					//performance cost
	std::string msg_s(msg);
	std::string tag_s(tag);

	tag_s += "]";
	tag_s = "[" + tag_s;

	msg_s = tag_s + msg_s;
	const char* full_m = msg_s.c_str();

	int old_size = Buf.size();
	va_list args;
	va_start(args, msg);
	Buf.appendv(full_m, args);
	va_end(tag);
	for (int new_size = Buf.size(); old_size < new_size; old_size++)
		if (Buf[old_size] == '\n')
			LineOffsets.push_back(old_size);
	ScrollToBottom = true;
	//visual studio log
	dbg(Buf.c_str());
#else
	(void)(true);
#endif
}

void CDebug::DrawLog()
{
#ifndef NDEBUG
	ImGui::SetNextWindowSize(ImVec2(512, 512), ImGuiSetCond_FirstUseEver);
	if (opened) {
		ImGui::Begin("Log", &opened);
		if (ImGui::Button("Clear")) Clear();
		ImGui::SameLine();
		bool copy = ImGui::Button("Copy");
		ImGui::SameLine();
		Filter.Draw("Filter", -100.0f);
		ImGui::Separator();
		ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
		if (copy) ImGui::LogToClipboard();

		if (Filter.IsActive())
		{
			const char* buf_begin = Buf.begin();
			const char* line = buf_begin;

			for (int line_no = 0; line != NULL; line_no++)
			{
				const char* line_end = (line_no < LineOffsets.size()) ? buf_begin + LineOffsets[line_no] : NULL;
				if (Filter.PassFilter(line, line_end)) {
					ImGui::TextUnformatted(line, line_end);
				}
				line = line_end && line_end[1] ? line_end + 1 : NULL;
			}
		}
		else
		{
			ImGui::TextUnformatted(Buf.begin());
		}

		if (ScrollToBottom)
			ImGui::SetScrollHere(1.0f);

		ScrollToBottom = false;
		ImGui::EndChild();
		ImGui::End();
	}
#endif
}

void CDebug::DrawLine(VEC3 org, VEC3 end, VEC3 color, float time)
{
#ifndef NDEBUG
	PROFILE_FUNCTION("Debug draw lines");
	if (time <= 0.f) {
		lines[next_line].org = org;
		lines[next_line].org = org;
		lines[next_line].end = end;
		lines[next_line++].color = color;
		assert(next_line < MAX_DBG_LINES);
	}
	else {
		lines_timed[next_timed_line].org = org;
		lines_timed[next_timed_line].org = org;
		lines_timed[next_timed_line].end = end;
		lines_timed[next_timed_line].color = color;
		lines_timed[next_timed_line++].time = time;
		assert(next_timed_line < MAX_DBG_TIMED_LINES);
	}
#endif
}

void CDebug::DrawLine(VEC3 pos, float dist, VEC3 direction, VEC3 color, float time)
{
#ifndef NDEBUG
	direction.Normalize();
	VEC3 pos_end = pos;
	pos_end += direction*dist;
	DrawLine(pos, pos_end, color, time);
#endif
}

void CDebug::RenderLine(line l)
{
#ifndef NDEBUG
	SimpleVertexColored vtxs_axis[2] =
	{
	  { l.org.x, l.org.y, l.org.z,    l.color.x, l.color.y, l.color.z, 1 },
	  { l.end.x, l.end.y, l.end.z,    l.color.x, l.color.y, l.color.z, 1 },
	};

	CMesh* mesh = new CMesh("line");
	if (mesh->create(2
		, sizeof(SimpleVertexColored)
		, vtxs_axis
		, 0, 0, nullptr
		, CMesh::VTX_DECL_POSITION_COLOR
		, CMesh::LINE_LIST
		, nullptr)) {
		mesh->activateAndRender();
	}

	mesh->destroy();
	delete mesh;
#endif
}

void CDebug::update(float dt) {
#ifndef NDEBUG
	console.update();
	DrawLog();
	if (controller->isDrawLineButtonPressed()) draw_lines = !draw_lines;
#endif
}

void CDebug::render()
{
#ifndef NDEBUG
	Render.activateBackBuffer();
	auto tech = Resources.get("solid_colored.tech")->as<CRenderTechnique>();
	tech->activate();
	if (draw_lines) {
		shader_ctes_object.World = MAT44::Identity;
		shader_ctes_object.uploadToGPU();
		//Render Lines
		//---------------------------------
		//No timed
		for (int i = 0; i < next_line; i++) {
			RenderLine(lines[i]);
		}

		//Timed
		for (int i = 0; i < next_timed_line; i++) {
			RenderLine(lines_timed[i]);
		}
		//----------------------------------
	}

	//Clear lines
	//----------------------------------
	//No timed
	next_line = 0;

	//Timed line
	int new_next_line = 0;
	int i = 0;
	while (i < next_timed_line) {
		lines_timed[i].time -= getDeltaTime();
		// Loop finish because in remove next_line--, otherwhise i++
		if (lines_timed[i].time <= 0.f) {
			removeTimedLine(i);
		}
		else {
			i++;
		}
	}
	//----------------------------------

	//line l = lines.back();
	//while (true) {
	//	if (io->keys['N'].isPressed() || io->keys['L'].isPressed()) {
	//		RenderLine(l);
	//	}
	//	lines.pop_back();
	//	if (lines.size() > 0) l = lines.back();
	//	else break;
	//}

	//lines.clear();
#endif
}

void CDebug::removeTimedLine(int i)
{
	next_timed_line--;
	assert(next_timed_line >= 0);
	if (i != next_timed_line) lines_timed[i] = lines_timed[next_line];
}