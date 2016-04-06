#include "mcv_platform.h"
#include "debug.h"
#include "render/mesh.h"
#include "resources/resource.h"

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
				if (Filter.PassFilter(line, line_end))
					ImGui::TextUnformatted(line, line_end);
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
}

void CDebug::DrawLine(VEC3 org, VEC3 end, VEC3 color)
{
	line new_line;
	new_line.org = org;
	new_line.end = end;
	new_line.color = color;

	lines.push_back(new_line);
}

void CDebug::DrawLine(VEC3 pos, VEC3 direction, float dist, VEC3 color)
{
	direction.Normalize();
	VEC3 pos_end = pos;
	pos_end += direction*dist;
	DrawLine(pos, pos_end, color);
}

void CDebug::RenderLine(line l)
{
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
}

void CDebug::update(float dt) {
	console.update();
	DrawLog();
}

void CDebug::render()
{
	if (lines.size() == 0) return;

	line l = lines.back();
	while (true) {
		RenderLine(l);
		lines.pop_back();
		if (lines.size() > 0) l = lines.back();
		else break;
	}
	lines.clear();
}