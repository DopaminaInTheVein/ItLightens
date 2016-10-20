#ifndef	INC_PARTICLES_SYSTEM_H_
#define INC_PARTICLES_SYSTEM_H_

#include "components\comp_base.h"
#include "ParticleData.h"
#include "render\render_instanced.h"
#include "ParticlesEmitter.h"

#define DEFAULT_PARTICLES_FILE "default_name"

using namespace physx;

class CalCoreSkeleton;

class CParticleSystem : public TCompBase {
  PxParticleSystem *			    m_pParticleSystem;
  PxParticleCreationData			m_particleData;

  int							    m_numParticles;
  int								m_numberFrames;

  PxVec3						    m_initial_pos;
  PxVec3						    m_initial_velocity;

  PxU32*							m_pParticleValidity;
  PxU32						        m_validParticleRange;
  float								m_initial_lifeTime;

  PxParticleExt::IndexPool*			m_pIndexPool;

  const CTexture*				    m_pTexture;

  TParticleData				        m_particles;
  CRenderParticlesInstanced			m_RenderParticles;
  CParticlesEmitter					m_Emitter;

  const CMesh *						m_pParticle_mesh;

  bool StepLifeTimeParticle(unsigned idx, float dt);
  //void SetParticleInitialValues(unsigned idx);

  bool active;
  bool loop;


  //debug ui values
  //------------------------------------------------------------------
  bool random_value_velocity = false;
  bool random_value_position = false;
  bool random_value_lifeTime = false;
  bool random_value_color	 = false;
  //------------------------------------------------------------------

  std::vector<int>	list_bones;
  std::vector<VEC3>	offset_bones;

  std::string  tex_particles_path;

  void UpdateRandomsAttr();
  

public:

  CParticleSystem() : m_pParticleSystem(nullptr), m_pParticleValidity(nullptr) {}
  ~CParticleSystem() {}

  void stop() {
	m_numParticles = 0;
    m_particles.clear();
	m_RenderParticles.clear();
	if(m_pParticleSystem) m_pParticleSystem->releaseParticles();
    PX_SAFE_RELEASE(m_pParticleSystem);
	PX_SAFE_RELEASE(m_pIndexPool);
	list_bones.clear();
	offset_bones.clear();
  }

  void render() {}	//not used
  void renderParticles();
  void init();
  bool CreateParticles(TParticleData& particles);

  void update(float elapsed);

  void updateParticlesWithoutPhysx(float elapsed);

  void SetTargetEmitter(std::string new_target);

  void ActiveParticleSystem();

  void updateParticlesPhysx(float elapsed);

  void UpdateAccelerationParticles();

  void SetPositionPhysxBuffer(std::vector<PxU32>& buffer);
  
  void SetBufferData();
  bool load(MKeyValue& atts);

  bool loadFileValues(MKeyValue & atts, std::string element);

  //-----------------------------------------------------------------------------------------------------
  //										Particles systems file manager
  //-----------------------------------------------------------------------------------------------------
  void saveToFile(std::string fileName);
  bool loadFromFile(std::string fileName);

  //-----------------------------------------------------------------------------------------------------
  //										Particles system editor
  //-----------------------------------------------------------------------------------------------------
  void renderInMenu();
  void RenderMenuSystemParticles();
  void RenderMenuSkeletonParticles();
  void printListChilds(int bone, CalCoreSkeleton * skeleton, std::vector<int>& bones_activated, int & idx);
  void ShowListBones(CEntity* owner, std::vector<int>& bones_activated);
};

#endif