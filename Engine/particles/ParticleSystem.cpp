#include "mcv_platform.h"
#include "ParticlesSystem.h"

#include "components\comp_transform.h"
#include "handle\handle.h"
#include "components\entity.h"
#include "render\draw_utils.h"
#include "resources\resources_manager.h"
#include "particles\particles_manager.h"
#include "ParticlesLoader.h"

#include "skeleton\comp_skeleton.h"
#include "skeleton\skeleton.h"

#include "app_modules\imgui\module_imgui.h"


#define P_OFF			TParticleData::State::OFF
#define P_WAITING		TParticleData::State::WAITING
#define P_STARTED		TParticleData::State::STARTED

#define BASE_PATH		std::string("data/particles/")

PxVec3 mulPxVec3(PxVec3 a, PxVec3 b) {
	PxVec3 ret;
	ret.x = a.x * b.x;
	ret.y = a.y * b.y;
	ret.z = a.z * b.z;
	return ret;
}

void CParticleSystem::renderParticles()
{
  m_pTexture->activate(TEXTURE_SLOT_DIFFUSE);
  m_RenderParticles.render();
}

bool CParticleSystem::load(MKeyValue & atts)
{
	std::string default_path = std::string( DEFAULT_PARTICLES_FILE) + ".particles";
  
  std::string file_particles = BASE_PATH;
  file_particles += atts.getString("file", default_path.c_str());
  CParticleLoader parser = CParticleLoader(CHandle(this));
  m_Emitter = CParticlesEmitter();
  bool is_ok = parser.xmlParseFile(file_particles);
  return is_ok;

  //return true;
}

bool CParticleSystem::loadFileValues(MKeyValue& atts, std::string element) {

	if (element == "particles_emitter") {
		m_numParticles = atts.getInt("num_particles", 1);
		m_particles = TParticleData(m_numParticles);
		m_Emitter = CParticlesEmitter();

		//if(m_RenderParticles.){
			//m_RenderParticles.clear();
		//}

		tex_particles_path = atts.getString("texture_particles", "textures/fire.dds");
		m_pTexture = Resources.get(tex_particles_path.c_str())->as<CTexture>();

		loop = atts.getBool("loop", false);
		active = atts.getBool("active", false);


		if(!m_pParticle_mesh ) m_pParticle_mesh = Resources.get("textured_quad_xy_centered.mesh")->as<CMesh>();

		m_Emitter.target_pos = atts.getString("target_pos", "mine");

		m_RenderParticles.create(m_numParticles, m_pParticle_mesh);
	}
	if (element == "nframes") {
		m_Emitter.SetNumberFrames(atts.getInt("value", 1));
		m_particles.max_frames = *m_Emitter.GetNumberFrames();

	}

	if (element == "collisions")
		m_Emitter.SetCollisions(atts.getBool("value", true));

	if(element == "gravity")
		m_Emitter.SetGravity(atts.getBool("value", false));

	if(element == "use_physx")
		m_Emitter.SetUsePhysx(atts.getBool("value", true));

	if (element == "shape") {
		m_Emitter.SetShape(atts.getInt("type", 0));
		m_Emitter.m_shape_emitter.angle = atts.getFloat("angle", 45);
		m_Emitter.m_shape_emitter.direction = atts.getPoint("direction");
		m_Emitter.m_shape_emitter.max = atts.getPoint("max");
		m_Emitter.m_shape_emitter.min = atts.getPoint("min");
	}

	if (element == "position")
	{
		VEC3 offset = VEC3(0,0,0);
		if (!(m_Emitter.target_pos == "mine")) {
			CEntity* e_target = tags_manager.getFirstHavingTag(m_Emitter.target_pos);
			if (e_target) {
				TCompTransform* t_target = e_target->get<TCompTransform>();
				if (t_target) {
					offset = t_target->getPosition();
				}
			}
		}
		else {
			CEntity* e_target = CHandle(this).getOwner();
			if (e_target) {
				TCompTransform* t_target = e_target->get<TCompTransform>();
				if (t_target) {
					offset = t_target->getPosition();
				}
			}
		}
		m_Emitter.SetPosition(PhysxConversion::Vec3ToPxVec3(atts.getPoint("value")));
		m_Emitter.SetPositionRandomMin(PhysxConversion::Vec3ToPxVec3(atts.getPoint("randmin")));
		m_Emitter.SetPositionRandomMax(PhysxConversion::Vec3ToPxVec3(atts.getPoint("randmax")));
	}

	if (element == "velocity")
	{
		m_Emitter.SetVelocity(PhysxConversion::Vec3ToPxVec3(atts.getPoint("value")));
		m_Emitter.SetVelocityRandomMin(PhysxConversion::Vec3ToPxVec3(atts.getPoint("randmin")));
		m_Emitter.SetVelocityRandomMax(PhysxConversion::Vec3ToPxVec3(atts.getPoint("randmax")));
		m_Emitter.SetVelocityModifier(PhysxConversion::Vec3ToPxVec3(atts.getPoint("vel_over_lifeTime")));
	}

	if (element == "size")
	{
		m_Emitter.SetSize(atts.getFloat("value", 1.0f));
		m_Emitter.SetModifierSize(atts.getFloat("modifier", 1.0f));
	}

	if (element == "lifetime")
	{
		m_Emitter.SetLifeTime(atts.getFloat("value", 1.0f));
		m_Emitter.SetLifeTimeRandmoMax(atts.getFloat("max_rand", 1.0f));
		m_Emitter.SetLifeTimeRandomMin(atts.getFloat("min_rand", 1.0f));
	}

	if (element == "acceleration")
	{
		m_Emitter.SetAcceleration(PhysxConversion::Vec3ToPxVec3(atts.getPoint("value")));
		m_Emitter.SetAccelModifier(PhysxConversion::Vec3ToPxVec3(atts.getPoint("modifier")));
	}
	if (element == "delay")
	{
		m_Emitter.SetDelayStart(atts.getFloat("delay_start", 0.0f));
		m_Emitter.SetRandomDelay(atts.getFloat("delay_particle_rand", 1.0f));
	}

	if (element == "color")
	{
		m_Emitter.SetColor(PhysxConversion::VEC4ToPxVec4(atts.getQuat("value")));
		m_Emitter.SetModifierColor(atts.getFloat("modifier", 1.0f));
	}

	if (element == "bonesList") {
		m_Emitter.m_useSkeleton = atts.getBool("value", false);
		
		//how many bones there are actives
		int num_bones = atts.getInt("size", 0);

		//loop to read active bones
		for (int i = 0; i < num_bones; i++) {
			std::string read = "bone" + std::to_string(i);

			//read bone id
			int num_bone = atts.getInt(read.c_str(), 0);

			//read offset bone
			read = "offset" + std::to_string(i);
			VEC3 offset = atts.getPoint(read.c_str());

			m_Emitter.AddBone(num_bone, offset);
			m_Emitter.AddParticleToBone(num_bone);
		}
	}

	return true;

}

void CParticleSystem::SetBufferData() {
  if(m_pParticleSystem)
	g_PhysxManager->GetActiveScene()->removeActor(*m_pParticleSystem);

  if (!m_Emitter.m_useSkeleton) {
	  m_pParticleSystem = g_PhysxManager->CreateParticleSystem(m_numParticles);
  }

  m_particles.initialize(m_numParticles);

  VEC3 offset = VEC3(0, 0, 0);
  if (!(m_Emitter.target_pos == "null")) {
	  CEntity* e_target = tags_manager.getFirstHavingTag(m_Emitter.target_pos);
	  if (e_target) {
		  TCompTransform* t_target = e_target->get<TCompTransform>();
		  if (t_target) {
			  offset = t_target->getPosition();
		  }
	  }
  }

  PxVec3 pos = PxVec3(0,0,0);

  for (int i = 0; i < m_numParticles; i++) {
    m_particles.indexBuffer[i] = i;
    m_particles.maxLifeTimeBuffer[i] = *m_Emitter.GetLifeTime() + random(*m_Emitter.GetLifeTimeRandomMin(), *m_Emitter.GetLifeTimeRandomMax()); //max time

	pos = *m_Emitter.GetPosition() + PhysxConversion::Vec3ToPxVec3(offset);
	m_particles.positionBuffer[i] = pos;
    m_particles.velocityBuffer[i] = *m_Emitter.GetVelocity();
    m_particles.negativeVelocityBuffer[i] = -*m_Emitter.GetVelocity();
    m_particles.lifeTimeBuffer[i] = m_particles.maxLifeTimeBuffer[i];
	dbg("Particle %d - max lifetime = %f\n", i, m_particles.lifeTimeBuffer[i]);

    m_particles.positionInitBuffer[i] = pos;
    m_particles.velocityInitBuffer[i] = m_initial_velocity;

	m_particles.colorBuffer[i] = PhysxConversion::PxVec4ToVec4(*m_Emitter.GetColor());
	m_particles.colorOriginBuffer[i] = m_particles.colorBuffer[i];
  }

  m_particles.numParticles = m_numParticles;
  
  bool ret = true;

  //m_pParticleValidity = (PxU32*)PX_ALLOC(((m_pParticleSystem->getMaxParticles() + 31) >> 5) << 2, "validParticleBitmap");
  if (!m_Emitter.m_useSkeleton) {
	  m_pIndexPool = PxParticleExt::createIndexPool(m_numParticles);
	  m_pParticleValidity = std::vector<PxU32>(((m_pParticleSystem->getMaxParticles() + 31) >> 5) << 2).data();
	  ret = CreateParticles(m_particles);
  }
  

  
  if (!ret) {
    fatal("particles not created\n");
    return;
  }
}

void CParticleSystem::init() {
  /*m_numParticles = 1;
  m_numberFrames = 1;
  int max_particles = 1;
  m_pParticleSystem = g_PhysxManager->CreateParticleSystem(max_particles);

  

  CEntity * e = CHandle(this).getOwner();
  TCompTransform *t = e->get<TCompTransform>();
  VEC3 curr_pos = t->getPosition();

  PxVec3 physx_pos = PhysxConversion::Vec3ToPxVec3(curr_pos);

  //initial values

  PxVec3 v = PxVec3(0, 5, 0);

  m_initial_pos = physx_pos;
  m_initial_velocity = v;

  m_Emitter = CParticlesEmitter(m_initial_pos);
  m_Emitter.SetVelocity(v);
  m_Emitter.SetUsePhysx(true);
  m_Emitter.SetColor(PxVec4(1,1,1,1));

  m_particles.initialize(max_particles);

  for (int i = 0; i < max_particles; i++) {
    int r = rand() % 100; //this produces numbers between -2000 - +2000
    m_initial_lifeTime = r / 10.0f;
    m_particles.indexBuffer[i] = i;
    m_particles.maxLifeTimeBuffer[i] = m_initial_lifeTime; //max time
    m_particles.positionBuffer[i] = m_initial_pos;
    m_particles.velocityBuffer[i] = m_initial_velocity;
    m_particles.negativeVelocityBuffer[i] = -m_initial_velocity;
    m_particles.lifeTimeBuffer[i] = m_particles.maxLifeTimeBuffer[i];

	m_particles.sizeBuffer[i] = 0.0f;

	m_particles.started[i] = false;
	m_particles.currDelayStart[i] = random(0.0f, *m_Emitter.GetRandomDelay()) + *m_Emitter.GetAbsoluteDelay();

    m_particles.positionInitBuffer[i] = m_initial_pos;
    m_particles.velocityInitBuffer[i] = m_initial_velocity;

	m_particles.colorBuffer[i] = PhysxConversion::PxVec4ToVec4(*m_Emitter.GetColor());
	m_particles.colorOriginBuffer[i] = m_particles.colorBuffer[i];
  }

  m_pIndexPool = PxParticleExt::createIndexPool(max_particles);
  //m_pParticleValidity = (PxU32*)PX_ALLOC(((m_pParticleSystem->getMaxParticles() + 31) >> 5) << 2, "validParticleBitmap");
  m_pParticleValidity = std::vector<PxU32>(((m_pParticleSystem->getMaxParticles() + 31) >> 5) << 2).data();

  m_particles.numParticles = max_particles;
  bool ret = CreateParticles(m_particles);
  if (!ret) {
    fatal("particles not created\n");
    return;
  }

  m_Emitter.m_useSkeleton = false;
  m_Emitter.SetSize(1.0f);
  m_Emitter.SetModifierSize(-1);

  g_particlesManager->AddParticlesSystem(this);

  m_RenderParticles.create(m_pParticleSystem->getMaxParticles(), m_pParticle_mesh);
  */


#ifndef FINAL_BUILD
	//Init editor values

  CEntity *player = tags_manager.getFirstHavingTag("player");
  if (!player)
	  return;


  TCompSkeleton *skel_player = player->get<TCompSkeleton>();
  if (!skel_player) return;

  auto skeleton = skel_player->model->getCoreModel()->getCoreSkeleton();
  auto bones = skeleton->getVectorCoreBone();


  //list_bones = new bool;
  list_bones.resize(bones.size());
  offset_bones.resize(bones.size());

  for (int i = 0; i < bones.size(); i++) {
	  list_bones[i] = 0;
	  offset_bones[i] = VEC3(0,0,0);
  }

  //update list_bones with real value
  //std::vector<int>* vector_bones = new std::vector<int>;
  //m_Emitter.GetListBones(vector_bones);

  for (auto value : m_Emitter.GetListBones()) {
	  list_bones[value] = 1;
  }

  //delete vector_bones;

#endif

	m_pParticle_mesh = Resources.get("textured_quad_xy_centered.mesh")->as<CMesh>();
	g_particlesManager->AddParticlesSystem(this);
  
}

bool CParticleSystem::CreateParticles(TParticleData& particles) {
  particles.numParticles = PxMin(particles.numParticles, m_pParticleSystem->getMaxParticles() - m_numParticles);
  particles.numParticles = m_numParticles;
  if (particles.numParticles > 0)
  {
    std::vector<PxU32> mTmpIndexArray;
    mTmpIndexArray.resize(particles.numParticles);
    PxStrideIterator<PxU32> indexData(&mTmpIndexArray[0]);
    // allocateIndices() may clamp the number of inserted particles
    particles.numParticles = m_pIndexPool->allocateIndices(particles.numParticles, indexData);

    PxParticleCreationData particleCreationData;
    particleCreationData.numParticles = particles.numParticles;
    particleCreationData.indexBuffer = PxStrideIterator<const PxU32>(&mTmpIndexArray[0]);
    particleCreationData.positionBuffer = PxStrideIterator<const PxVec3>(&particles.positionBuffer[0]);
    particleCreationData.velocityBuffer = PxStrideIterator<const PxVec3>(&particles.velocityBuffer[0]);

    //m_numParticles += particles.numParticles;
    //m_numParticles = particles.numParticles;
    bool ok = m_pParticleSystem->createParticles(particleCreationData);

    if (*m_Emitter.GetGravity())
      m_pParticleSystem->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, false);
    else
      m_pParticleSystem->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

    PX_UNUSED(ok);
    PX_ASSERT(ok);

    return ok;
  }
  
  return false;
}

bool CParticleSystem::StepLifeTimeParticle(unsigned idx, float dt)
{
  m_particles.lifeTimeBuffer[idx] -= dt;
  if (m_particles.lifeTimeBuffer[idx] <= 0)
    return false;

  return true;
}

PxU32 lowestSetBit(PxU32 b) {
  if (b) {
    unsigned long retval;
    _BitScanForward(&retval, b);
    return retval;
  }
  return 0;
}

void CParticleSystem::update(float elapsed) {
	if (m_particles.indexBuffer.size() <= 0) return;
	if (!m_Emitter.m_useSkeleton)
		updateParticlesPhysx(elapsed);
	else
		updateParticlesWithoutPhysx(elapsed);

	//update particle render data
  m_RenderParticles.update(elapsed, m_particles);
}

void CParticleSystem::updateParticlesWithoutPhysx(float elapsed) {
	//updateParticlesPhysx(elapsed);
	for (int idx = 0; idx<m_Emitter.GetNumBonesUsed(); idx++) {
		int bone_id = m_Emitter.GetBoneId(idx);
		if (bone_id < 0 || m_particles.indexBuffer.size() <= idx)	//bone or particle invalid
			continue;

		bool to_update = true;
		bool removed = false;

		//start delay
		if (m_particles.started[idx] == P_WAITING) {

			//particle has not started, waiting to start
			removed = true;

			if (m_particles.currDelayStart[idx] <= 0.0f) {

				//particle start time finished, particle to start
				m_particles.sizeBuffer[idx] = *m_Emitter.GetSize();
				m_particles.lifeTimeBuffer[idx] = m_particles.maxLifeTimeBuffer[idx];
				m_particles.started[idx] = P_STARTED;
			}

			//update delay start timer
			else {
				m_particles.currDelayStart[idx] -= elapsed;
				m_particles.sizeBuffer[idx] = 0.0f;	//inivisible particle
				to_update = false;
			}
		}
		else {
			//if (loop) {
				if (m_particles.lifeTimeBuffer[idx] <= 0.0f) {
					m_particles.currDelayStart[idx] = random(0.0f, *m_Emitter.GetRandomDelay()) + *m_Emitter.GetAbsoluteDelay();
					m_particles.started[idx] = P_WAITING;
					m_particles.sizeBuffer[idx] = 0.0f;
					to_update = false;
				}
			//}
		}

		//update running particles
		if(to_update) {
			m_particles.positionBuffer[idx] = PhysxConversion::Vec3ToPxVec3(m_Emitter.GetPosBone(bone_id));
			m_particles.sizeBuffer[idx] = *m_Emitter.GetSize();
			m_particles.started[idx] = P_STARTED;

			//add dt to particle lifetime
			if (m_particles.maxLifeTimeBuffer[idx] > 0)
			{
				m_particles.lifeTimeBuffer[idx] -= elapsed;
			}
		}
	}
}

void CParticleSystem::SetTargetEmitter(std::string new_target) {
	m_Emitter.target_pos = new_target;
}

static std::vector<int> particles_finished;
void CParticleSystem::ActiveParticleSystem() {
	particles_finished.clear();
	particles_finished.resize(m_numParticles);
	VEC3 offset = VEC3(0, 0, 0);
	if (!(m_Emitter.target_pos == "null")) {
		CEntity* e_target = tags_manager.getFirstHavingTag(m_Emitter.target_pos);
		if (e_target) {
			TCompTransform* t_target = e_target->get<TCompTransform>();
			if (t_target) {
				offset = t_target->getPosition();
			}
		}
	}


	PxVec3 pos = PxVec3(0, 0, 0);


	std::vector<PxU32> buffer;

	for (int i = 0; i < m_numParticles; i++) {
		m_particles.indexBuffer[i] = i;
		m_particles.maxLifeTimeBuffer[i] = *m_Emitter.GetLifeTime() + random(*m_Emitter.GetLifeTimeRandomMin(), *m_Emitter.GetLifeTimeRandomMax()); //max time

		pos = *m_Emitter.GetPosition() + PhysxConversion::Vec3ToPxVec3(offset);
		m_particles.positionBuffer[i] = pos;
		m_particles.velocityBuffer[i] = *m_Emitter.GetVelocity();
		m_particles.negativeVelocityBuffer[i] = -*m_Emitter.GetVelocity();
		m_particles.lifeTimeBuffer[i] = m_particles.maxLifeTimeBuffer[i];
		dbg("Particle %d - max lifetime = %f\n", i, m_particles.lifeTimeBuffer[i]);

		m_particles.positionInitBuffer[i] = pos;
		m_particles.velocityInitBuffer[i] = m_initial_velocity;

		m_particles.colorBuffer[i] = PhysxConversion::PxVec4ToVec4(*m_Emitter.GetColor());
		m_particles.colorOriginBuffer[i] = m_particles.colorBuffer[i];

		m_particles.currDelayStart[i] = random(0.0f, *m_Emitter.GetRandomDelay()) + *m_Emitter.GetAbsoluteDelay();
		dbg("Particle %d - max delay = %f\n", i, m_particles.currDelayStart[i]);

		m_particles.started[i] = P_WAITING;

		buffer.push_back(i);
	}

	SetPositionPhysxBuffer(buffer);
}


void CParticleSystem::updateParticlesPhysx(float elapsed) {
  VEC3 forward = VEC3(0, 0, 1);
  if (!m_pParticleSystem)
	  return;

  PxParticleReadData *data = m_pParticleSystem->lockParticleReadData();
  //m_numParticles = 0;

  std::vector<PxU32> mTmpIndexArray;
  PxU32 newValidRange = 0;
  bool to_remove = false;



  if (data->validParticleRange > 0)
  {
    PxStrideIterator<const PxVec3> positions(data->positionBuffer);
    PxStrideIterator<const PxVec3> velocities(data->velocityBuffer);
    PxStrideIterator<const PxParticleFlags> particleFlags(data->flagsBuffer);

    // copy particles positions
	for (int idx = 0; idx < m_numParticles; idx++){
			bool removed = false;

			if (m_particles.started[idx] == P_OFF) {
				m_particles.colorBuffer[idx].w = 0.0f;	//inivisible particle
				continue;
			}

			//start delay
			if (m_particles.started[idx] == P_WAITING){
				removed = true;
				if (m_particles.currDelayStart[idx] <= 0.0f) {
					m_particles.sizeBuffer[idx] = *m_Emitter.GetSize();
					m_particles.started[idx] = P_STARTED;
					to_remove = true;
					mTmpIndexArray.push_back(idx);
				}
				else {
					m_particles.currDelayStart[idx] -= elapsed;
					m_particles.colorBuffer[idx].w = 0.0f;	//inivisible particle
				}
			}

			//check if particle has time limit
			if (m_particles.maxLifeTimeBuffer[idx] > 0 && !removed && m_particles.started[idx] != P_OFF)
			{
				if ((std::find(particles_finished.begin(), particles_finished.end(), idx) != particles_finished.end())) {
					int i = 0;
				}
				//check if particle has surprassed its time limit
				if (m_particles.lifeTimeBuffer[idx] <= 0.0f)
				{
					if (loop) {
						m_particles.lifeTimeBuffer[idx] = m_particles.maxLifeTimeBuffer[idx];
						m_particles.currDelayStart[idx] = random(0.0f, *m_Emitter.GetRandomDelay()) + *m_Emitter.GetAbsoluteDelay();
						m_particles.started[idx] = P_WAITING;
					}
					else
					{
						m_particles.started[idx] = P_OFF;
						particles_finished.push_back(idx);
						m_particles.colorBuffer[idx] = VEC4(0,0,0,0);
					}

					

					//generate random start time
					mTmpIndexArray.push_back(idx);
					removed = true;
					to_remove = true;
				}
			}
	

			//if particle is not removed
			if (!removed)
			{
				  m_particles.positionBuffer[idx] = positions[idx];
				  m_particles.velocityBuffer[idx] = velocities.ptr() ? velocities[idx] : PxVec3(0.0f);
				  m_particles.negativeVelocityBuffer[idx] = -m_particles.velocityBuffer[idx];
				  if (*m_Emitter.GetModifierColor() != 1) {
					  m_particles.colorBuffer[idx].x = m_particles.colorOriginBuffer[idx].x * m_particles.lifeTimeBuffer[idx]* *m_Emitter.GetModifierColor();
					  m_particles.colorBuffer[idx].y = m_particles.colorOriginBuffer[idx].y * m_particles.lifeTimeBuffer[idx] * *m_Emitter.GetModifierColor();
					  m_particles.colorBuffer[idx].z = m_particles.colorOriginBuffer[idx].z * m_particles.lifeTimeBuffer[idx] * *m_Emitter.GetModifierColor();
				  }
				  m_particles.colorBuffer[idx].w = m_particles.colorOriginBuffer[idx].w * m_particles.lifeTimeBuffer[idx];

				  //m_particles.colorBuffer[idx] = max(m_);

				  //add dt to particle lifetime
				  if (m_particles.maxLifeTimeBuffer[idx] > 0)
				  {
						m_particles.lifeTimeBuffer[idx] -= elapsed;
				  }

				  //draw particle, disabled for now
				  //VEC3 pos = PhysxConversion::PxVec3ToVec3(positions[idx]);
				  //Debug->DrawLine(pos, forward, 2);

				  //m_numParticles++;
				  newValidRange = idx;
			}
		}
  }
  //dbg("end\n");

  data->unlock();

  UpdateAccelerationParticles();

  //free removed particles
  if (loop) {
	  if (to_remove)
	  {
		  SetPositionPhysxBuffer(mTmpIndexArray);
	  }
  }

  //bool ret = CreateParticles(m_particles);
  //if (!ret) {
    //fatal("particles not created\n");
  //return;
  //}
 
}

void CParticleSystem::UpdateAccelerationParticles() {
	std::vector<PxU32> buffer;
	std::vector<PxVec3> buffer_accels;
	for (int i = 0; i < m_numParticles; i++) {
		buffer.push_back(i);

		PxVec3 vel_modif = (*m_Emitter.GetVelocityModifier());
		PxVec3 accel = mulPxVec3(vel_modif, m_particles.negativeVelocityBuffer[i]) * (m_particles.maxLifeTimeBuffer[i] - m_particles.lifeTimeBuffer[i]);

		buffer_accels.push_back(accel);
	}

	PxStrideIterator<const PxU32> indexData(&buffer[0]);
	PxStrideIterator<const PxVec3> accelData(&buffer_accels[0]);

	m_pParticleSystem->addForces(static_cast<PxU32>(buffer.size()), indexData, accelData, PxForceMode::eACCELERATION);
}

void CParticleSystem::SetPositionPhysxBuffer(std::vector<PxU32>& buffer) {
	UpdateRandomsAttr();

	PxStrideIterator<const PxU32> indexData(&buffer[0]);

	//m_pParticleSystem->releaseParticles(static_cast<PxU32>(mTmpIndexArray.size()), indexData);
	//m_pIndexPool->freeIndices(static_cast<PxU32>(mTmpIndexArray.size()), indexData);

	auto initPos = PxStrideIterator<const PxVec3>(&m_particles.positionInitBuffer[0]);
	auto initVel = PxStrideIterator<const PxVec3>(&m_particles.velocityInitBuffer[0]);

	auto negativeVel = PxStrideIterator<const PxVec3>(&m_particles.negativeVelocityBuffer[0]);

	m_pParticleSystem->setPositions(static_cast<PxU32>(buffer.size()), indexData, initPos);
	m_pParticleSystem->setVelocities(static_cast<PxU32>(buffer.size()), indexData, initVel);

	//m_pParticleSystem->addForces(static_cast<PxU32>(mTmpIndexArray.size()), indexData, negativeVel, PxForceMode::eVELOCITY_CHANGE);
}

void CParticleSystem::UpdateRandomsAttr() {

	CEntity* e = CHandle(this).getOwner();
	VEC3 front, up;
	if (!e) {
		front = VEC3(0,0,1);
		up = VEC3(0,1,0);
	}
	else {
		TCompTransform* t = e->get<TCompTransform> ();
		front = t->getFront();
		up = t->getUp();
	}

  if (random_value_position) {
    for (int i = 0; i < m_numParticles; i++) {
      PxVec3 pos_random = *m_Emitter.GetPosition();
      PxVec3 max_limit = *m_Emitter.GetPositionRandomMax();
      PxVec3 min_limit = *m_Emitter.GetPositionRandomMin();

      pos_random.x += random(max_limit.x, min_limit.x);
      pos_random.y += random(max_limit.y, min_limit.y);
      pos_random.z += random(max_limit.z, min_limit.z);

	  m_particles.colorBuffer[i].x = m_particles.colorOriginBuffer[i].x;
	  m_particles.colorBuffer[i].y = m_particles.colorOriginBuffer[i].y;
	  m_particles.colorBuffer[i].z = m_particles.colorOriginBuffer[i].z;
      m_particles.positionInitBuffer[i] = pos_random;
    }
  }
  else {
	  for (int i = 0; i < m_numParticles; i++) {
		  m_particles.positionInitBuffer[i] = m_Emitter.GetInitialPosByShape(front, up);
		  m_particles.colorBuffer[i].x = m_particles.colorOriginBuffer[i].x;
		  m_particles.colorBuffer[i].y = m_particles.colorOriginBuffer[i].y;
		  m_particles.colorBuffer[i].z = m_particles.colorOriginBuffer[i].z;
	  }
  }

  if (random_value_velocity) {
    for (int i = 0; i < m_numParticles; i++) {
      PxVec3 vel_random = *m_Emitter.GetVelocity();
      PxVec3 max_limit = *m_Emitter.GetVelocityRandomMax();
      PxVec3 min_limit = *m_Emitter.GetVelocityRandomMin();

      vel_random.x += random(max_limit.x, min_limit.x);
      vel_random.y += random(max_limit.y, min_limit.y);
      vel_random.z += random(max_limit.z, min_limit.z);

      m_particles.velocityInitBuffer[i] = vel_random;
	}
 
  }
  else {
	  for (int i = 0; i < m_numParticles; i++) {
		  m_particles.velocityInitBuffer[i] = m_Emitter.GetInitialVelByShape(front, up, m_particles.positionInitBuffer[i] );
	  }
  }
}

//#########################################################################################################
//									File manager functions
//#########################################################################################################

#pragma region File manager functions

void CParticleSystem::saveToFile(std::string fileName)
{
	std::string full_path = "data/particles/" + fileName + ".particles";

	MKeyValue atts;

	//file buffer
	std::filebuf fb;
	fb.open(full_path.c_str(), std::ios::out);
	std::ostream file(&fb);

	//start_element
	atts.put("num_particles", m_numParticles);
	atts.put("target_pos", m_Emitter.target_pos);

	atts.put("texture_particles", tex_particles_path);

	atts.put("loop", loop);
	atts.put("active", active);

	atts.writeStartElement(file, "particles_emitter");
	atts.clear();

	//number particles
	atts.put("value", *m_Emitter.GetNumberFrames());
	atts.writeSingle(file, "nframes");
	atts.clear();

	//collisions
	atts.put("value", *m_Emitter.GetCollisions());
	atts.writeSingle(file, "collisions");
	atts.clear();

	//gravity
	atts.put("value", *m_Emitter.GetGravity());
	atts.writeSingle(file, "gravity");
	atts.clear();

	//use_physx
	atts.put("value", *m_Emitter.GetUsePhysx());
	atts.writeSingle(file, "use_physx");
	atts.clear();

	//shape
	atts.put("type", *m_Emitter.GetShape());
	atts.put("angle", m_Emitter.m_shape_emitter.angle);
	atts.put("radius", m_Emitter.m_shape_emitter.radius);
	atts.put("max", m_Emitter.m_shape_emitter.max);
	atts.put("min", m_Emitter.m_shape_emitter.min);
	atts.writeSingle(file, "shape");
	atts.clear();

	//position
	PxVec3 data = *m_Emitter.GetPosition();
	atts.put("value", PhysxConversion::PxVec3ToVec3(data));
	data = *m_Emitter.GetPositionRandomMax();
	atts.put("randmax", PhysxConversion::PxVec3ToVec3(data));
	data = *m_Emitter.GetPositionRandomMin();
	atts.put("randmin", PhysxConversion::PxVec3ToVec3(data));
	atts.writeSingle(file ,"position");
	atts.clear();

	//velocity
	data = *m_Emitter.GetVelocity();
	atts.put("value", PhysxConversion::PxVec3ToVec3(data));
	data = *m_Emitter.GetVelocityRandomMax();
	atts.put("randmax", PhysxConversion::PxVec3ToVec3(data));
	data = *m_Emitter.GetVelocityRandomMin();
	atts.put("randmin", PhysxConversion::PxVec3ToVec3(data));
	data = *m_Emitter.GetVelocityModifier();
	atts.put("vel_over_lifeTime", PhysxConversion::PxVec3ToVec3(data));
	atts.writeSingle(file, "velocity");
	atts.clear();

	//size
	atts.put("value", *m_Emitter.GetSize());
	atts.put("modifier", *m_Emitter.GetModifierSize());
	atts.writeSingle(file, "size");
	atts.clear();

	//lifetime
	atts.put("value", *m_Emitter.GetLifeTime());
	atts.put("max_rand", *m_Emitter.GetLifeTimeRandomMax());
	atts.put("min_rand", *m_Emitter.GetLifeTimeRandomMin());
	atts.writeSingle(file, "lifetime");
	atts.clear();

	//acceleration
	data = *m_Emitter.GetAcceleration();
	atts.put("value", PhysxConversion::PxVec3ToVec3(data));
	data = *m_Emitter.GetAccelModifier();
	atts.put("modifier", PhysxConversion::PxVec3ToVec3(data));
	atts.writeSingle(file, "acceleration");
	atts.clear();

	//delay
	atts.put("delay_start", *m_Emitter.GetDelayStart());
	atts.put("delay_particle_rand", *m_Emitter.GetRandomDelay());
	atts.writeSingle(file, "delay");
	atts.clear();

	//color
	atts.put("value", PhysxConversion::PxVec4ToVec4(*m_Emitter.GetColor()));
	atts.put("modifier", *m_Emitter.GetModifierColor());
	atts.writeSingle(file, "color");
	atts.clear();

	//bones
	auto lb = m_Emitter.GetListBones();
	if (lb.size() > 0) {
		atts.put("value", true);
	}
	else {
		atts.put("value", false);
	}

	int size = lb.size();
	atts.put("size", size);

	for (int i = 0; i < lb.size(); i++) {
		std::string name = "bone" + std::to_string(i);
		atts.put(name.c_str(), lb[i]);
	}

	atts.writeSingle(file, "bonesList");
	atts.clear();

	//end element
	atts.writeEndElement(file, "particles_emitter");
	
	fb.close();
	

}

bool CParticleSystem::loadFromFile(std::string fileName)
{
	CParticleLoader ep = CParticleLoader(CHandle(this));
	bool is_ok = ep.xmlParseFile(fileName);
	if (!is_ok) {
		fatal("error reading particle system %s file, the file wont be loaded\n", fileName.c_str());
		return false;
	}

	return true;
}

#pragma endregion

//#########################################################################################################
//									Particles System editor & debug
//#########################################################################################################

#pragma region Particles System editor & debug

void CParticleSystem::renderInMenu()
{

  ImGui::Text("num particles: %d\n", m_numParticles);

  ImGui::Separator();

  if (ImGui::TreeNode("Particles System editor")) {

	  if (ImGui::Checkbox("loop", &loop)) {
		  ActiveParticleSystem();
	  }
	  if (ImGui::Checkbox("active", &active)) {
		  ActiveParticleSystem();
		  active = false;
	  }

    if (ImGui::Checkbox("collisions(only with PC)", m_Emitter.GetCollisions())) {
      bool collisions = *m_Emitter.GetCollisions();
      if (collisions) {
        m_pParticleSystem->setParticleBaseFlag(PxParticleBaseFlag::eCOLLISION_WITH_DYNAMIC_ACTORS, true);
      }
      else {
        m_pParticleSystem->setParticleBaseFlag(PxParticleBaseFlag::eCOLLISION_WITH_DYNAMIC_ACTORS, false);
      }
    }

	if (ImGui::Checkbox("Use movement simulation(Physx)", m_Emitter.GetUsePhysx())) {
		//nothing
	}

    if (ImGui::Checkbox("gravity", m_Emitter.GetGravity())) {
      bool gravity = *m_Emitter.GetGravity();
      m_pParticleSystem->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !gravity);
    }

    if (ImGui::DragInt("Number particles", &m_numParticles, 1)) {
      m_RenderParticles.clear();
      m_RenderParticles.create(m_numParticles, m_pParticle_mesh);
      SetBufferData();
    }

	char text[64];
	strcpy(text, m_Emitter.target_pos.c_str());
	ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
	if (ImGui::InputText("target position", text, 64, flags)) {
		m_Emitter.target_pos = std::string(text);

		VEC3 offset = VEC3(0, 0, 0);
		if (!(m_Emitter.target_pos == "null")) {
			CEntity* e_target = tags_manager.getFirstHavingTag(m_Emitter.target_pos);
			if (e_target) {
				TCompTransform* t_target = e_target->get<TCompTransform>();
				if (t_target) {
					offset = t_target->getPosition();
				}
			}
		}

		//m_Emitter.SetPosition(*m_Emitter.GetPosition()+ PhysxConversion::Vec3ToPxVec3(offset));
		m_RenderParticles.clear();
		m_RenderParticles.create(m_numParticles, m_pParticle_mesh);
		SetBufferData();
	}

	ImGui::Text("\n");

	ImGui::Checkbox("attach particles to skeleton", &m_Emitter.m_useSkeleton);

	if (ImGui::SmallButton("clear all bones")) {
		for (int i = 0; i < list_bones.size(); i++) {
			list_bones[i] = 0;
			offset_bones[i] = VEC3(0, 0, 0);
		}
	}

	if (ImGui::SmallButton("Select all bones")) {
		for (int i = 0; i < list_bones.size(); i++) {
			list_bones[i] = 1;
		}
	}

	if (m_Emitter.m_useSkeleton) {

		RenderMenuSkeletonParticles();

	}
	else {
		RenderMenuSystemParticles();
	}
	
	ImGui::Text("Shape type emitter particles\n");
	
	ImGui::RadioButton("Nothing", m_Emitter.GetShape(), CParticlesEmitter::SHAPE_NOTHING);

	ImGui::RadioButton("Box", m_Emitter.GetShape(), CParticlesEmitter::SHAPE_BOX);
	if (*m_Emitter.GetShape()== CParticlesEmitter::SHAPE_BOX) {
		ImGui::DragFloat3("max point box", &m_Emitter.m_shape_emitter.max.x, 0.02f);
		ImGui::DragFloat3("min point box", &m_Emitter.m_shape_emitter.min.x, 0.02f);
	}

	ImGui::RadioButton("Sphere", m_Emitter.GetShape(), CParticlesEmitter::SHAPE_SPHERE);
	if (*m_Emitter.GetShape() == CParticlesEmitter::SHAPE_SPHERE) {
		ImGui::DragFloat("radius", &m_Emitter.m_shape_emitter.radius);
	}

	ImGui::RadioButton("Edge", m_Emitter.GetShape(), CParticlesEmitter::SHAPE_EDGE);
	if (*m_Emitter.GetShape() == CParticlesEmitter::SHAPE_EDGE) {
		ImGui::DragFloat3("direction edge", &m_Emitter.m_shape_emitter.direction.x, 0.02f);
	}

	ImGui::RadioButton("Cone", m_Emitter.GetShape(), CParticlesEmitter::SHAPE_CONE);
	if (*m_Emitter.GetShape() == CParticlesEmitter::SHAPE_CONE) {
		ImGui::DragFloat3("vector up", &m_Emitter.m_shape_emitter.direction.x, 0.02f);
		ImGui::DragFloat("radius base", &m_Emitter.m_shape_emitter.radius);
		ImGui::DragFloat("half_angle", &m_Emitter.m_shape_emitter.angle);
	}
	
	ImGui::RadioButton("Circle", m_Emitter.GetShape(), CParticlesEmitter::SHAPE_CIRCLE);
	if (*m_Emitter.GetShape() == CParticlesEmitter::SHAPE_CIRCLE) {
		ImGui::DragFloat3("vector up", &m_Emitter.m_shape_emitter.direction.x, 0.02f);
		ImGui::DragFloat("radius", &m_Emitter.m_shape_emitter.radius);
	}
	

	ImGui::Text("\n");

    ImGui::Checkbox("random velocity range", &random_value_velocity);
    if (!random_value_velocity) {
      if (ImGui::DragFloat3("initial velocity particles", &m_Emitter.GetVelocity()->x, 0.1f)) {
		  m_initial_velocity = *m_Emitter.GetVelocity();
        for (int i = 0; i < m_particles.numParticles; i++) {
          m_particles.velocityInitBuffer[i] = m_initial_velocity;
        }
      }
    }
    else {
      ImGui::Text("Random vector between 2 vectors");
      if (ImGui::DragFloat3("max limit random", &m_Emitter.GetVelocityRandomMax()->x)) {
        //nothing
      }
      if (ImGui::DragFloat3("min limit random", &m_Emitter.GetVelocityRandomMin()->x)) {
        //nothing
      }
    }

	ImGui::Text("\n");

    ImGui::Checkbox("random position range", &random_value_position);
    if (!random_value_position) {
      if (ImGui::DragFloat3("initial position particles", &m_Emitter.GetPosition()->x, 0.1f)) {
		  m_initial_pos = *m_Emitter.GetPosition();
        for (int i = 0; i < m_particles.numParticles; i++) {
          m_particles.positionInitBuffer[i] = *m_Emitter.GetPosition();
        }
      }
    }
    else {
      ImGui::Text("Random vector between 2 vectors");
      if (ImGui::DragFloat3("max limit random", &m_Emitter.GetPositionRandomMax()->x)) {
        //nothing
      }
      if (ImGui::DragFloat3("min limit random", &m_Emitter.GetPositionRandomMin()->x)) {
        //nothing
      }
    }

	ImGui::Text("\n");
	ImGui::Checkbox("random color range", &random_value_color);
	if (!random_value_color) {
		
		if (ImGui::DragFloat4("color influence particles", &m_Emitter.GetColor()->x, 0.1f)) {
			for (int i = 0; i < m_particles.numParticles; i++) {
				m_particles.colorBuffer[i] = PhysxConversion::PxVec4ToVec4(*m_Emitter.GetColor());
			}
		}
	}
	else {
		/*ImGui::Text("Random vector between 2 vectors");
		if (ImGui::DragFloat3("max limit random", &m_Emitter.GetVelocityRandomMax()->x)) {
			//nothing
		}
		if (ImGui::DragFloat3("min limit random", &m_Emitter.GetVelocityRandomMin()->x)) {
			//nothing
		}//here end*/
	}

	ImGui::DragFloat("modifier color over lifeTime", m_Emitter.GetModifierColor());

	ImGui::Text("\n");

    ImGui::Checkbox("random lifeTime range", &random_value_lifeTime);
    if (!random_value_lifeTime) {
      if (ImGui::DragFloat("initial lifeTime particles", &m_initial_lifeTime, 0.1f)) {
        //changes applied when particles born again
		  m_Emitter.SetLifeTime(m_initial_lifeTime);
        for (int i = 0; i < m_particles.numParticles; i++) {
          m_particles.maxLifeTimeBuffer[i] = m_initial_lifeTime;
        }
      }
    }
    else {
      ImGui::Text("Random vector between 2 values");
      if (ImGui::DragFloat("max limit random", m_Emitter.GetLifeTimeRandomMax())) {
        //changes applied when particles born again
        for (int i = 0; i < m_particles.numParticles; i++) {
          m_particles.maxLifeTimeBuffer[i] = random(*m_Emitter.GetLifeTimeRandomMin(), *m_Emitter.GetLifeTimeRandomMax());
        }
      }
      if (ImGui::DragFloat("min limit random", m_Emitter.GetLifeTimeRandomMin())) {
        //changes applied when particles born again
        for (int i = 0; i < m_particles.numParticles; i++) {
          m_particles.maxLifeTimeBuffer[i] = random(*m_Emitter.GetLifeTimeRandomMin(), *m_Emitter.GetLifeTimeRandomMax());
        }
      }
    }

	if (ImGui::DragFloat("initial random delay particles", m_Emitter.GetRandomDelay(), 0.1f)) {
		//nothing
	}

	ImGui::Text("\n");

    ImGui::DragFloat("size", m_Emitter.GetSize());

	ImGui::DragFloat("size modifier, change on lifetime", m_Emitter.GetModifierSize());

	

	static std::string file_texture = "NOT_TO_LOAD";
	ImGui::Text("\n");
	if (ImGui::SmallButton("Load particles texture")) {
		file_texture = CImGuiModule::getFilePath();
		auto p = file_texture.find_last_of("\\");		//get last part from path
		file_texture = file_texture.substr(p+1);

		if (file_texture != "") {		//valid entry
		
			file_texture = "textures/" + file_texture;
			//int i = 0;
			m_pTexture = Resources.get(file_texture.c_str())->as<CTexture>();
		}
	}
	if (file_texture != "NOT_TO_LOAD") {


		file_texture = "NOT_TO_LOAD";
	}

	if (ImGui::DragInt("number frames texture", m_Emitter.GetNumberFrames())) {
		m_particles.max_frames = *m_Emitter.GetNumberFrames();
	}

    ImGui::TreePop();
  }
}

void CParticleSystem::RenderMenuSystemParticles() {
	
	if (!m_pParticleSystem)
		return;

	PxParticleReadData *data = m_pParticleSystem->lockParticleReadData();

	PxStrideIterator<const PxVec3> positions(data->positionBuffer);
	PxStrideIterator<const PxVec3> velocities(data->velocityBuffer);
	PxStrideIterator<const PxParticleFlags> particleFlags(data->flagsBuffer);

	ImGui::Separator();

	ImGui::Text("Particles info");

	if (ImGui::TreeNode("Particles info")) {
		for (unsigned idx = 0; idx < data->validParticleRange; ++idx, ++particleFlags, ++positions, ++velocities) {
			if (data->validParticleBitmap[idx] & PxParticleFlag::eVALID) {
				std::string title = "particle: " + std::to_string(idx);

				//particle data
				if (ImGui::TreeNode(title.c_str())) {
					ImGui::Text("Particle position: %f, %f, %f\n", positions[idx].x, positions[idx].y, positions[idx].z);
					ImGui::Text("Particle velocity: %f, %f, %f\n", velocities[idx].x, velocities[idx].y, velocities[idx].z);
					ImGui::Text("Particle lifetime: %f\n", m_particles.lifeTimeBuffer[idx]);
					ImGui::Text("Particle delay start; %f\n", m_particles.currDelayStart[idx]);

					ImGui::TreePop();
				}
			}
		}

		ImGui::TreePop();
	}

	data->unlock();

}

void CParticleSystem::RenderMenuSkeletonParticles() {
	static std::string name_bone = "nothing";
	static VEC3 traslacion = VEC3(0, 0, 0);
	std::vector<int> bones_activated;		//list of bones id with particles
	m_Emitter.ClearBones();

	if (ImGui::TreeNode("Show bones list")) {
		CEntity *player = tags_manager.getFirstHavingTag("player");
		if(player)
			ShowListBones(player, bones_activated);

		ImGui::TreePop();
	}

	//only valid 1 particle by bone or 1 particle system at 1 bone
	if (bones_activated.size() == 0) return;
	if (bones_activated.size() > 1) {	

		//reset num_particles
		//m_RenderParticles.clear();
		//m_RenderParticles.create(m_numParticles, m_pParticle_mesh);
		//SetBufferData();
	}
	else {
		PxVec3 new_position = PhysxConversion::Vec3ToPxVec3(m_Emitter.testBones(bones_activated[0], &name_bone, &traslacion));
		ImGui::Text("position for bone %s is: %f, %f, %f\n", name_bone.c_str(), new_position.x, new_position.y, new_position.z);
		ImGui::Text("traslacion of bone from core is: %f, %f, %f\n", traslacion.x, traslacion.y, traslacion.z);
		for (int i = 0; i < m_particles.numParticles; i++) {
			m_particles.positionInitBuffer[i] = new_position;
			//m_Emitter.SetPosition(new_position);
		}
	}
}

void CParticleSystem::printListChilds(int bone, CalCoreSkeleton* skeleton, std::vector<int>& bones_activated, int& idx) {
	auto childs = skeleton->getCoreBone(bone)->getListChildId();
	bool test = false;
	for (int child : childs) {
		std::string name = "bone:" + std::to_string(child) + " - " + skeleton->getCoreBone(child)->getName();
		bool checked = list_bones[idx];
		VEC3 offset = offset_bones[idx];
		if (ImGui::Checkbox(name.c_str(), &checked)) {
			if (checked) {
				list_bones[idx] = 1;
			}
			else {
				list_bones[idx] = 0;
				offset_bones[idx] = VEC3(0,0,0);
			}
		}
		if (checked) {

			//offset bone
			std::string name_offset = name + " offset";
			ImGui::DragFloat3(name_offset.c_str(), &offset_bones[idx].x);

			//add info to emitter
			bones_activated.push_back(child);
			m_Emitter.AddBone(child, offset_bones[idx]);
		}
		idx++;
		printListChilds(child, skeleton, bones_activated, idx);
	}
}

void resetOwner(std::string tag) {

}

void CParticleSystem::ShowListBones(CEntity* owner, std::vector<int>& bones_activated) {
	assert(owner);
	TCompSkeleton *skel_player = owner->get<TCompSkeleton>();
	if (!skel_player) return;

	auto skeleton = skel_player->model->getCoreModel()->getCoreSkeleton();

	auto bones_root = skeleton->getVectorRootCoreBoneId();

	int idx = 0;
	for (int bone_root : bones_root) {
		std::string name = "bone:" + std::to_string(bone_root) + " - " + skeleton->getCoreBone(bone_root)->getName();
		bool checked = list_bones[idx];
		if (ImGui::Checkbox(name.c_str(), &checked)) {
			if (checked) {
				list_bones[idx] = 1;
			}
			else {
				list_bones[idx] = 0;
				offset_bones[idx] = VEC3(0,0,0);
			}
		}
		if (checked) {

			//offset bone
			std::string name_offset = name + " offset";
			ImGui::DragFloat3(name_offset.c_str(), &offset_bones[idx].x);

			//Add info to emitter
			bones_activated.push_back(bone_root);
			m_Emitter.AddBone(bone_root, offset_bones[idx]);
		}

		idx++;
		printListChilds(bone_root, skeleton, bones_activated, idx);
		if (m_numParticles != bones_activated.size()) {
			m_numParticles = bones_activated.size();
			
			//reset num_particles
			m_RenderParticles.clear();
			m_RenderParticles.create(m_numParticles, m_pParticle_mesh);
			//SetBufferData();

			SetBufferData();
		}
	}

}

#pragma endregion