#ifndef INC_PARTICLES_LOADER
#define INC_PARTICLES_LOADER

class CParticleSystem;

class CParticleLoader : public CXMLParser {
	ClHandle				m_handle;
	CParticleSystem*	m_pParticles_system;

public:
	CParticleLoader() : m_pParticles_system(nullptr) { m_handle = ClHandle(); }
	CParticleLoader(ClHandle particles_handle) : m_pParticles_system(nullptr) { m_handle = particles_handle; }
	~CParticleLoader() {}
	void onStartElement(const std::string &elem, MKeyValue &atts) override;
	void onEndElement(const std::string &elem) override;
};

#endif