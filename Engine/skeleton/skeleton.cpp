#include "mcv_platform.h"
#include "skeleton.h"
#include "cal3d/cal3d.h"
#include "resources/resources_manager.h"

#include "render/mesh.h"
#include "render/mesh_format.h"
#include "utils/data_saver.h"

using namespace MeshFormat;

template<> IResource::eType getTypeOfResource<CSkeleton>() { return IResource::SKELETON; }

template<>
IResource* createObjFromName<CSkeleton>(const std::string& name) {
	CSkeleton* obj = new CSkeleton;
	auto full_name = IResource::getDataPath() + name;
	if (!obj->xmlParseFile(full_name))
		return nullptr;
	obj->setName(name.c_str());
	return obj;
}

// ---------------------------------------------
struct TSkinVertex {
	float pos[3];
	float normal[3];
	float uv[2];
	unsigned char bone_ids[4];
	unsigned char weights[4];
};

void convertToEngineFormat(CalCoreModel* core_model, int mesh_id, const std::string& ofilename) {
	CalCoreMesh* core_mesh = core_model->getCoreMesh(mesh_id);
	int num_sub_meshes = core_mesh->getCoreSubmeshCount();

	assert(num_sub_meshes == 1);

	CMemoryDataSaver ds;
	CMemoryDataSaver ds_vtxs;
	CMemoryDataSaver ds_idxs;
	CMemoryDataSaver ds_groups;

	// Preparar el header del fichero de salida
	THeader header;
	header.version = THeader::current_version;
	header.bytes_per_idx = sizeof(uint16_t);
	header.bytes_per_vtx = sizeof(TSkinVertex);
	header.num_groups = num_sub_meshes;
	header.num_idxs = 0;
	header.num_vtxs = 0;
	header.primitive_type = CMesh::TRIANGLE_LIST;
	header.the_magic_terminator = magic_terminator;
	header.vertex_type = CMesh::VTX_DECL_POSITION_NORMAL_UV_SKIN;

	VEC3 pmin;
	VEC3 pmax;

	// For each material in this core_mesh
	for (int sub_mesh_id = 0; sub_mesh_id < num_sub_meshes; ++sub_mesh_id) {
		CalCoreSubmesh* core_sub_mesh = core_mesh->getCoreSubmesh(sub_mesh_id);

		// # faces & indices
		int nfaces = core_sub_mesh->getFaceCount();
		auto& cal_faces = core_sub_mesh->getVectorFace();

		for (auto& f : cal_faces) {
			// Swap culling mode from MAX
			ds_idxs.write(f.vertexId[0]);
			ds_idxs.write(f.vertexId[2]);
			ds_idxs.write(f.vertexId[1]);
		}

		// # vertexs and vertexs data
		int nvertexs = core_sub_mesh->getVertexCount();
		auto& cal_vtxs = core_sub_mesh->getVectorVertex();

		// An array of all textcoord sets
		auto& cal_all_uvs_sets = core_sub_mesh->getVectorVectorTextureCoordinate();

		// We must have at least one texture coordinate set
		std::vector<CalCoreSubmesh::TextureCoordinate>* cal_uvs = nullptr;

		// Use the first texture coordinate set
		if (!cal_all_uvs_sets.empty())
			cal_uvs = &cal_all_uvs_sets[0];

		// For each vertex in this submesh
		int vtx_id = 0;
		for (auto& v : cal_vtxs) {
			TSkinVertex skin_vtx;
			memset(&skin_vtx, 0x00, sizeof(TSkinVertex));

			// Cal info
			auto cal_pos = v.position;
			auto cal_normal = v.normal;

			// Our vertex
			skin_vtx.pos[0] = cal_pos.x;
			skin_vtx.pos[1] = cal_pos.y;
			skin_vtx.pos[2] = cal_pos.z;
			skin_vtx.normal[0] = cal_normal.x;
			skin_vtx.normal[1] = cal_normal.y;
			skin_vtx.normal[2] = cal_normal.z;

			// Texture coords...
			if (cal_uvs) {
				skin_vtx.uv[0] = cal_uvs->operator[](vtx_id).u;
				skin_vtx.uv[1] = cal_uvs->operator[](vtx_id).v;
			}

			// For each influcence affecting this vertex
			int total_w = 0;
			int ninfluence = 0;
			for (auto& cal_influence : v.vectorInfluence) {
				auto bone_id = cal_influence.boneId;    // bone_id
				auto weight = cal_influence.weight;     // 0...1

				assert(bone_id < 256);
				skin_vtx.bone_ids[ninfluence] = (uint8_t)(bone_id);
				skin_vtx.weights[ninfluence] = (uint8_t)(weight * 255);
				total_w += skin_vtx.weights[ninfluence];
				++ninfluence;
				if (ninfluence >= 4)
					break;
			}

			if (total_w != 255) {
				assert(total_w < 255);
				skin_vtx.weights[0] += (255 - total_w);
			}

			// Save just the vertex info, not the riff
			ds_vtxs.write(skin_vtx);

			// Keep AABB updated
			VEC3 ploc(cal_pos.x, cal_pos.y, cal_pos.z);
			if (sub_mesh_id == 0 && vtx_id == 0) {
				pmin = pmax = ploc;
			}
			else {
				pmin = VEC3::Min(pmin, ploc);
				pmax = VEC3::Max(pmax, ploc);
			}

			++vtx_id;
		}

		// Define a new group
		// CAUTION comentado!
		CMesh::TGroup g;
		g.first_index = header.num_idxs;
		g.num_indices = cal_faces.size() * 3; // each face -> 3 idxs
		ds_groups.write(g);

		// Update header info
		header.num_idxs += cal_faces.size() * 3;
		header.num_vtxs += nvertexs;
	}

	// Our virtual file...
	// The mesh header
	TRiff riff;
	riff.magic = magic_header;
	riff.num_bytes = sizeof(THeader);
	ds.write(riff);
	ds.write(header);

	// The vertex chunk
	riff.magic = magic_vtxs;
	riff.num_bytes = ds_vtxs.size();
	ds.write(riff);
	ds.writeBytes(ds_vtxs.getDataBase(), ds_vtxs.size());

	// The faces chunk
	riff.magic = magic_idxs;
	riff.num_bytes = ds_idxs.size();
	ds.write(riff);
	ds.writeBytes(ds_idxs.getDataBase(), ds_idxs.size());

	// All groups - CAUTION comentado
	riff.magic = magic_groups;
	riff.num_bytes = ds_groups.size();
	ds.write(riff);
	ds.writeBytes(ds_groups.getDataBase(), ds_groups.size());

	// The terminator
	riff.magic = magic_mesh_end;
	riff.num_bytes = 0;
	ds.write(riff);

	bool is_ok = ds.writeToFile(ofilename.c_str());
	assert(is_ok);
	dbg("Skin mesh has AABB (%f,%f,%f)-(%f,%f,%f)\n", pmin.x, pmin.y, pmin.z, pmax.x, pmax.y, pmax.z);
}

// ---------------------------------------------

void CSkeleton::onStartElement(const std::string &elem, MKeyValue &atts) {
	auto src = IResource::getDataPath() + atts.getString("src", "");
	if (elem == "skeleton") {
		bool is_ok = core_model->loadCoreSkeleton(src);
		assert(is_ok);
	}
	else if (elem == "animation") {
		int anim_id = core_model->loadCoreAnimation(src);
		core_model->getCoreAnimation(anim_id)->setName(src);
		assert(anim_id != -1);
	}
	else if (elem == "animations") {
		WIN32_FIND_DATA ffd;
		std::string pattern = src + "*.caf";
		HANDLE hFind = FindFirstFile(pattern.c_str(), &ffd);
		do {
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) {
				dbg("File is %s\n", ffd.cFileName);
				std::string name(ffd.cFileName);    // idle.CAF
				std::string full_src = src + name;  // skels/barrufet/idle.caf
													// Remove extension
				name.resize(name.size() - 4);
				int anim_id = core_model->loadCoreAnimation(full_src);
				assert(anim_id != -1);
				saveAnimId(src, anim_id);
				assert(anim_id < MAX_NUMBER_ANIMS);
				core_model->getCoreAnimation(anim_id)->setName(name);
			}
		} while (FindNextFile(hFind, &ffd) != 0);
		FindClose(hFind);
	}
	else if (elem == "mesh") {
		std::string mesh_file = src;
		if (src.find(".cmf")) {
			int mesh_id = core_model->loadCoreMesh(src);
			core_model->getCoreMesh(mesh_id)->setName(src);
			assert(mesh_id != -1);
			mesh_file.resize(mesh_file.size() - 4);
			mesh_file += ".mesh";
			convertToEngineFormat(core_model, mesh_id, mesh_file);
			mesh_file = mesh_file.substr(strlen(IResource::getDataPath()));
		}
		// load the mesh_file
		//auto engine_mesh = Resources.get(mesh_file.c_str())->as<CMesh>();
	}
}

void CSkeleton::saveAnimId(std::string src, int anim_id) {
	// Size of nameAnims is enough
	assert(anim_id < MAX_NUMBER_ANIMS);
	// Save anim name
	nameAnims[anim_id] = src;
}
int CSkeleton::getAnimIdByName(std::string name) const {
	//Looking for the name in saved animations
	for (int i = 0; i < MAX_NUMBER_ANIMS; ++i) {
		if (nameAnims[i] == name) return i;
	}
	// Doesnt exist that name!
	return -1;
}

CSkeleton::CSkeleton()
	: core_model(nullptr)
{
	CalLoader::setLoadingMode(LOADER_ROTATE_X_AXIS | LOADER_INVERT_V_COORD);
	core_model = new CalCoreModel("unknown");
}

void CSkeleton::destroy()
{
	delete core_model;
}