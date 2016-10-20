#ifndef INC_COMPONENT_PARTICLE_SYSTEM_MESH_H_
#define INC_COMPONENT_PARTICLE_SYSTEM_MESH_H_

#include "comp_base.h"

class CParticleSystem;

struct TCompParticleSystem : public TCompBase {
	const CParticleSystem* particle_system;
	std::string res_name;
	bool load(MKeyValue& atts);
	bool loadFromFile(std::string);
	bool save(std::ofstream& os, MKeyValue& atts);
	void saveToFile(std::string s);
	void renderInMenu();
	void init();
	void stop();
	void renderParticles();
	void render();
};

#endif
