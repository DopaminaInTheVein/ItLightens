#ifndef INC_RESOURCE_SKELETON_H_
#define INC_RESOURCE_SKELETON_H_

#include "resources/resource.h"
#include "utils/XMLParser.h"
#include "cal3d/cal3d.h"
#include "skeleton_manager.h"

#define MAX_NUMBER_ANIMS	64

class CSkeleton : public IResource, public CXMLParser {
	CCoreModel*   core_model;

	std::string nameAnims[MAX_NUMBER_ANIMS];
	void saveAnimId(std::string src, int anim_id);

	void onStartElement(const std::string &elem, MKeyValue &atts);

	std::map<std::string, std::string> key_bones;
public:
	CSkeleton();
	eType getType() const { return SKELETON; }

	std::string getKeyBoneName(std::string);

	bool isValid() const {
		return core_model != nullptr;
	}
	void destroy();

	CalCoreModel* getCoreModel() { return core_model; }
	int getAnimIdByName(std::string name) const;

	static void CalculateTangentArray(
		int vertexCount, std::vector<CalCoreSubmesh::Vertex> *vertex,
		std::vector<std::vector<CalCoreSubmesh::TextureCoordinate>> *texcoord,
		int triangleCount, std::vector<CalCoreSubmesh::Face> *triangle,
		// Output
		std::vector<VEC4> *tangent
	);

	struct TBoneCorrector {
		int         bone_id;
		CalVector   local_dir;
		float       local_amount;
		bool        render;
		TBoneCorrector() : bone_id(-1), local_dir(1, 0, 0) {}
		TBoneCorrector(int abone_id, CalVector alocal_dir)
			: bone_id(abone_id)
			, local_dir(alocal_dir)
		{}
		void apply(CalModel* model, CalVector target, float amount);
	};

	typedef std::vector <TBoneCorrector> VBoneCorrections;
	VBoneCorrections bone_corrections;
};
#endif
