#include "mcv_platform.h"
#include "resources/resources_manager.h"
#include "components/entity.h"
#include "comp_particle_system.h"
#include "particles\ParticlesSystem.h"

#define PS_RESOURCE (const_cast<CParticleSystem*>(particle_system))

bool TCompParticleSystem::load(MKeyValue& atts)
{
	res_name = atts.getString("file", "");
	particle_system = Resources.get(res_name.c_str())->as<CParticleSystem>();
	assert(particle_system);

	return true;
}
bool TCompParticleSystem::loadFromFile(std::string file)
{
	res_name = file;
	particle_system = Resources.get(res_name.c_str())->as<CParticleSystem>();
	assert(particle_system);

	return true;
}

bool TCompParticleSystem::save(std::ofstream& os, MKeyValue& atts)
{
	atts.put("file", res_name);
	return true;
}

void TCompParticleSystem::saveToFile(std::string file)
{
	PS_RESOURCE->saveToFile(file);
}

void TCompParticleSystem::renderInMenu()
{
	PS_RESOURCE->renderUIDebug();
}

void TCompParticleSystem::init()
{
	PS_RESOURCE->init();
}

void TCompParticleSystem::stop()
{
	PS_RESOURCE->stop();
}

void TCompParticleSystem::render()
{
	PS_RESOURCE->renderParticles();
}

//void TCompParticleSystem::renderParticles()
//{
//
//}