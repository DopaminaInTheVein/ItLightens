#ifndef INC_MESH_PHYSX_H_
#define	INC_MESH_PHYSX_H_

struct FileDataMesh;

#include <vector>
#include <map>

class CMeshPhysxManager {
public:
	std::map<std::string, int> m_index_meshes;
	std::vector<FileDataMesh*> m_meshes;

	void initMeshManager() {
		m_index_meshes = std::map<std::string, int>();
		m_meshes = std::vector<FileDataMesh*>();
	}

	void addNewMeshPhysx(FileDataMesh* new_mesh, std::string mesh_name);
	void deleteMesh();
	bool getPhysxMesh(std::string mesh_name);

	FileDataMesh * loadPhysxMesh(std::string mesh_name);

	void clearDataMeshManager() {
		m_index_meshes.clear();
		m_meshes.clear();
	}

};

#endif