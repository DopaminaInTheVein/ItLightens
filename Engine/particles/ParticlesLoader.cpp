#include "mcv_platform.h"
#include "ParticlesLoader.h"

#include "ParticlesSystem.h"

void CParticleLoader::onStartElement(const std::string & elem, MKeyValue & atts)
{
	m_pParticles_system = m_handle;
	if (!m_pParticles_system) {
		return;
	}

	if (elem == "particles_emitter") {
		m_pParticles_system->loadFileValues(atts, elem);
	}
}

void CParticleLoader::onEndElement(const std::string & elem)
{
	//nothing
}
