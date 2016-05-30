#ifndef	INC_PARTICLES_SYSTEM_H_
#define INC_PARTICLES_SYSTEM_H_

#include "components\comp_base.h"
#include "ParticleData.h"
#include "render\render_instanced.h"
#include "ParticlesEmitter.h"

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

  bool StepLifeTimeParticle(unsigned idx, float max_time, float dt);
  //void SetParticleInitialValues(unsigned idx);

  


  //debug ui values
  //------------------------------------------------------------------
  bool random_value_velocity = false;
  bool random_value_position = false;
  bool random_value_lifeTime = false;
  bool random_value_color	 = false;
  //------------------------------------------------------------------

  std::vector<int> list_bones;

  void UpdateRandomsAttr();
  void SetBufferData();

public:

  CParticleSystem() : m_pParticleSystem(nullptr), m_pParticleValidity(nullptr) {}
  ~CParticleSystem() {}

  void stop() {
    m_particles.clear();
    PX_SAFE_RELEASE(m_pParticleSystem);
	PX_SAFE_RELEASE(m_pIndexPool);
  }

  void render() {}	//not used
  void renderParticles();
  void init();
  bool CreateParticles(TParticleData& particles);
  void update(float elapsed);
  
  bool load(MKeyValue& atts);

  //-----------------------------------------------------------------------------------------------------
  //										Particles systems file manager
  //-----------------------------------------------------------------------------------------------------
  void saveToFile(std::string fileName);
  void loadFromFile(std::string fileName);

  //-----------------------------------------------------------------------------------------------------
  //										Particles system editor
  //-----------------------------------------------------------------------------------------------------
  void renderInMenu();
  void RenderMenuSkeletonParticles();
  void printListChilds(int bone, CalCoreSkeleton * skeleton, std::vector<int>& bones_activated, int & idx);
  void ShowListBones(CEntity* owner, std::vector<int>& bones_activated);
};

#endif