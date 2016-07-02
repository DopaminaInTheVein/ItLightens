#include "mcv_platform.h"
#include "resource.h"
#include "resources_manager.h"
#include "render/mesh.h"
#include "render/technique.h"
#include "render/texture.h"
#include "render/material.h"
#include "render/static_mesh.h"
#include "imgui/imgui.h"
#include "skeleton/skeleton.h"

#include "app_modules/logic_manager/logic_manager.h"

CResourcesManager Resources;

const char* IResource::getTypeName(IResource::eType atype) {
	switch (atype) {
	case IResource::UNDEFINED: return "undefined";
	case IResource::MESH:      return "Meshes";
	case IResource::TECHNIQUE: return "Techniques";
	case IResource::VERTEX_SHADER: return "VertexShaders";
	case IResource::PIXEL_SHADER: return "PixelShaders";
	case IResource::TEXTURE: return "Textures";
	case IResource::MATERIAL: return "Materials";
	case IResource::STATIC_MESH: return "StaticMeshes";
	case IResource::SKELETON: return "Skeletons";
	}
	return "invalid";
}

void CResourcesManager::onFileChanged(const std::string& filename) {
	std::string filename_correct = filename;
	std::replace(filename_correct.begin(), filename_correct.end(), '\\', '/'); // replace all 'x' to 'y'
	dbg("Resources file %s changed!\n", filename.c_str());

	//Check Extensions
	std::string ext(filename_correct);
	auto p = ext.find_last_of(".");
	if (p == std::string::npos) return;
	ext = ext.substr(p);

	//Is LUA file
	if (ext == ".lua") {
		logic_manager->reloadFile(filename_correct);
	}
	// Other files
	else {
		for (auto it : all)
			it.second->onFileChanged(filename_correct);
	}
}

const IResource* CResourcesManager::get(const char* name) {
	// Do we have this object?
	auto it = all.find(name);

	// Yes, we have...
	if (it != all.end())
		return it->second;

	// Try to load...
	std::string ext(name);
	auto p = ext.find_last_of(".");
	if (p == std::string::npos) {
		return nullptr;
	}
	ext = ext.substr(p);

	IResource* new_obj = nullptr;

	if (ext == ".mesh") {
		new_obj = createObjFromName<CMesh>(name);
	}
	else if (ext == ".tech") {
		new_obj = createObjFromName<CRenderTechnique>(name);
	}
	else if (ext == ".vs") {
		new_obj = createObjFromName<CVertexShader>(name);
	}
	else if (ext == ".ps") {
		new_obj = createObjFromName<CPixelShader>(name);
	}
	else if (ext == ".dds" || ext == ".DDS") {
#ifndef NDEBUG
		std::string str = std::string(name);
		int found = str.find("caja_2_difuso");
		if (found != std::string::npos)
			dbg("Aqui estas\n");
#endif
		new_obj = createObjFromName<CTexture>(name);
	}
	else if (ext == ".material") {
		new_obj = createObjFromName<CMaterial>(name);
	}
	else if (ext == ".static_mesh") {
		new_obj = createObjFromName<CStaticMesh>(name);
	}
	else if (ext == ".skeleton") {
		new_obj = createObjFromName<CSkeleton>(name);
	}
	else {
		fatal("Invalid resource type %s at %s\n", ext.c_str(), name);
	}

	all[name] = new_obj;
	return new_obj;
}

// -------------------------------------
void CResourcesManager::registerNew(IResource* new_res) {
	assert(new_res);
	assert(!new_res->getName().empty());
	all[new_res->getName()] = new_res;
}

void CResourcesManager::renderUIDebug(ImGuiTextFilter * filter) {
	std::vector<bool> res_filter;
	res_filter.resize(IResource::NUM_RESOURCES_TYPE);

	for (auto& it : res_filter)
		it = false;

	for (int i = IResource::UNDEFINED + 1; i < IResource::NUM_RESOURCES_TYPE; ++i) {
		auto res_type = (IResource::eType)(i);
		bool show = false;
		const char* res_type_name = IResource::getTypeName(res_type);
		for (auto it : all) {
			auto r = it.second;
			if (r->getType() == res_type) {
				if (filter->PassFilter(it.first.c_str())) {
					res_filter[i] = true;
				}
			}
		}
	}

	//All resources loop
	std::vector<std::string> resources;
	for (int i = IResource::UNDEFINED + 1; i < IResource::NUM_RESOURCES_TYPE; ++i) {
		if (res_filter[i] == true) {
			auto res_type = (IResource::eType)(i);
			bool show = false;
			const char* res_type_name = IResource::getTypeName(res_type);
			if (ImGui::TreeNode(res_type_name)) { //type resources header
				for (auto it : all) {
					auto r = it.second;
					if (r->getType() == res_type) {
						if (filter->PassFilter(it.first.c_str())) {
							if (ImGui::TreeNode((it.first.c_str()))) {
								r->renderUIDebug();
								if (ImGui::SmallButton("reload"))
									r->reload();
								ImGui::TreePop();
							}
						}
					}
				}
				ImGui::TreePop();
			}
		}
	}

	res_filter.clear();
}