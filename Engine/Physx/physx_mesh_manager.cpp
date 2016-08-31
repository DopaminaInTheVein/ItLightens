#include "mcv_platform.h"
#include "physx_mesh_manager.h"

#include "render\mesh.h"

void CMeshPhysxManager::addNewMeshPhysx(FileDataMesh* new_mesh, std::string mesh_name)
{

	if (getPhysxMesh(mesh_name)) {
		return;
	}

	int id = m_meshes.size();
	m_meshes.push_back(new_mesh);

	//std::map<std::string, int>::iterator it = m_index_meshes.end();
	m_index_meshes.insert(std::pair<std::string, int>(mesh_name, id));  // max efficiency inserting

	//m_index_meshes.insert(new_mesh, id);
	//m_index_meshes[mesh_name] = id;
}

void CMeshPhysxManager::deleteMesh()
{
	//TODO
}

bool CMeshPhysxManager::getPhysxMesh(std::string mesh_name)
{
	if (m_index_meshes.find(mesh_name) == m_index_meshes.end())
	{
		return false;
	}
	else {
		/*mesh_output = m_meshes[m_index_meshes[mesh_name]];
		if (!mesh_output->numVtx) {

			mesh_output = m_meshes[0];
		}*/
		return true;
	}
}

FileDataMesh* CMeshPhysxManager::loadPhysxMesh(std::string mesh_name)
{

		FileDataMesh* mesh_output = m_meshes[m_index_meshes[mesh_name]];
		return mesh_output;
}


