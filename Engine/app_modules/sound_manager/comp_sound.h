#ifndef INC_SOUND_COMPONENT_H_
#define	INC_SOUND_COMPONENT_H_

#include "components/comp_base.h"
#include "components/comp_transform.h"
#include "components/comp_hierarchy.h"

#include "handle/handle.h"
#include "sound_manager.h"

using namespace FMOD;

struct TCompSound : public TCompBase {
private:
	void updateHierarchy();
public:
	CHandle mParent;
	VEC3 entity_position;
	std::string entity_name;
	//TCompHierarchy* hierarchy_comp;

	// loaded atributes
	std::string event = "OnFluoriscent";
	float volume = 0.25f;

	// aux
	bool sound_playing = false;
	float MAX_DISTANCE = 20.f;

	Studio::EventInstance* sound_instance;

	void init();
	void update(float elapsed);
	bool load(MKeyValue& atts);
	bool save(std::ofstream& os, MKeyValue& atts);
	~TCompSound();
};

#endif