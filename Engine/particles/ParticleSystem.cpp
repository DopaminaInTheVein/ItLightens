#include "mcv_platform.h"
#include "ParticlesSystem.h"

#include "components\comp_transform.h"
#include "handle\handle.h"
#include "components\entity.h"
#include "render\draw_utils.h"
#include "resources\resources_manager.h"
#include "particles\particles_manager.h"

void CParticleSystem::renderParticles()
{
  m_pTexture->activate(TEXTURE_SLOT_DIFFUSE);
  m_RenderParticles.render();
}

bool CParticleSystem::load(MKeyValue & atts)
{
	std::string tex_particles = atts.getString("texture_particles", "textures/fire.dds");
	m_pTexture = Resources.get(tex_particles.c_str())->as<CTexture>();

	return true;
}

void CParticleSystem::SetBufferData() {

	g_PhysxManager->GetActiveScene()->removeActor(*m_pParticleSystem);

	m_pParticleSystem = g_PhysxManager->CreateParticleSystem(m_numParticles);

	m_particles.initialize(m_numParticles);

	for (int i = 0; i < m_numParticles; i++) {
		m_particles.indexBuffer[i] = i;
		m_particles.maxLifeTimeBuffer[i] = m_initial_lifeTime; //max time
		m_particles.positionBuffer[i] = m_initial_pos;
		m_particles.velocityBuffer[i] = m_initial_velocity;
		m_particles.negativeVelocityBuffer[i] = -m_initial_velocity;
		m_particles.lifeTimeBuffer[i] = m_particles.maxLifeTimeBuffer[i];

		m_particles.positionInitBuffer[i] = m_initial_pos;
		m_particles.velocityInitBuffer[i] = m_initial_velocity;
	}

	m_pIndexPool = PxParticleExt::createIndexPool(m_numParticles);
	//m_pParticleValidity = (PxU32*)PX_ALLOC(((m_pParticleSystem->getMaxParticles() + 31) >> 5) << 2, "validParticleBitmap");
	m_pParticleValidity = std::vector<PxU32>(((m_pParticleSystem->getMaxParticles() + 31) >> 5) << 2).data();

	m_particles.numParticles = m_numParticles;
	bool ret = CreateParticles(m_particles);
	if (!ret) {
		fatal("particles not created\n");
		return;
	}
}

void CParticleSystem::init() {
  m_numParticles = 250;
  int max_particles = 250;
  m_pParticleSystem = g_PhysxManager->CreateParticleSystem(max_particles);
  
  m_pParticle_mesh = Resources.get("textured_quad_xy_centered.mesh")->as<CMesh>();

  CEntity * e = CHandle(this).getOwner();
  TCompTransform *t = e->get<TCompTransform>();
  VEC3 curr_pos = t->getPosition();

  PxVec3 physx_pos = PhysxConversion::Vec3ToPxVec3(curr_pos);

  //initial values

  PxVec3 v = PxVec3(0, 5, 0);

  m_initial_pos = physx_pos;
  m_initial_velocity = v;

  m_Emitter = TParticlesEmitter(m_initial_pos);

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

    m_particles.positionInitBuffer[i] = m_initial_pos;
    m_particles.velocityInitBuffer[i] = m_initial_velocity;
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

  g_particlesManager->AddParticlesSystem(this);

  m_RenderParticles.create(m_pParticleSystem->getMaxParticles(), m_pParticle_mesh);
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

	if(*m_Emitter.GetGravity())
		m_pParticleSystem->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, false);  
	else
		m_pParticleSystem->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

    PX_UNUSED(ok);
    PX_ASSERT(ok);

    return ok;
  }

  return false;
}

bool CParticleSystem::StepLifeTimeParticle(unsigned idx, float max_time, float dt)
{
  m_particles.lifeTimeBuffer[idx] += dt;
  if (m_particles.lifeTimeBuffer[idx] >= max_time)
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
  VEC3 forward = VEC3(0, 0, 1);
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
    PxMemCopy(m_pParticleValidity, data->validParticleBitmap, ((data->validParticleRange + 31) >> 5) << 2);
	
    // copy particles positions
	for (PxU32 w = 0; w <= (data->validParticleRange - 1) >> 5; w++)
	{
		for (PxU32 b = data->validParticleBitmap[w]; b; b &= b - 1)
		{
			bool removed = false;
			PxU32 idx = (w << 5 | lowestSetBit(b));

			//check if particle has time limit
			if (m_particles.maxLifeTimeBuffer[idx] > 0)
			{
			  //check if particle has surprassed its time limit
			  if (m_particles.lifeTimeBuffer[idx] <= 0.0f)
			  {
				m_particles.lifeTimeBuffer[idx] = m_particles.maxLifeTimeBuffer[idx];
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

			//if removed mark particle as invalid
			//else
		//	{
			 // m_pParticleValidity[w] &= (b - 1);
			//}

			/*dbg("%f-", m_pParticleValidity[w]);

			if (data->validParticleBitmap[w])
				dbg("%f ",data->validParticleBitmap[w]);
			else
				dbg("%f ", data->validParticleBitmap[w]);*/
		}
	}
  }
  dbg("end\n");
  

  m_validParticleRange = newValidRange;
  data->unlock();

  //free removed particles
  if (to_remove)
  {

	  UpdateRandomsAttr();

    PxStrideIterator<const PxU32> indexData(&mTmpIndexArray[0]);

    //m_pParticleSystem->releaseParticles(static_cast<PxU32>(mTmpIndexArray.size()), indexData);
    //m_pIndexPool->freeIndices(static_cast<PxU32>(mTmpIndexArray.size()), indexData);

    auto initPos = PxStrideIterator<const PxVec3>(&m_particles.positionInitBuffer[0]);
    auto initVel = PxStrideIterator<const PxVec3>(&m_particles.velocityInitBuffer[0]);



	auto negativeVel = PxStrideIterator<const PxVec3>(&m_particles.negativeVelocityBuffer[0]);

    m_pParticleSystem->setPositions(static_cast<PxU32>(mTmpIndexArray.size()), indexData, initPos);
	m_pParticleSystem->setVelocities(static_cast<PxU32>(mTmpIndexArray.size()), indexData, initVel);
    //m_pParticleSystem->addForces(static_cast<PxU32>(mTmpIndexArray.size()), indexData, negativeVel, PxForceMode::eVELOCITY_CHANGE);
  }

  //bool ret = CreateParticles(m_particles);
  //if (!ret) {
    //fatal("particles not created\n");
	//return;
  //}

  //update particle render data
  m_RenderParticles.update(elapsed, m_particles);
}

void CParticleSystem::UpdateRandomsAttr() {
	for (int i = 0; i < m_numParticles; i++) {

	}
}

void CParticleSystem::renderInMenu()
{
	PxParticleReadData *data = m_pParticleSystem->lockParticleReadData();

	ImGui::Text("num valid particles: %d\n", data->nbValidParticles);
	ImGui::Text("valid range: %d\n", data->validParticleRange);

	PxStrideIterator<const PxVec3> positions(data->positionBuffer);
	PxStrideIterator<const PxVec3> velocities(data->velocityBuffer);
	PxStrideIterator<const PxParticleFlags> particleFlags(data->flagsBuffer);

	ImGui::Separator();

	if (ImGui::TreeNode("Particles System editor")) {

		if (ImGui::Checkbox("collisions(only with PC)", m_Emitter.GetCollisions())) {
			bool collisions = *m_Emitter.GetCollisions();
			if (collisions) {
				m_pParticleSystem->setParticleBaseFlag(PxParticleBaseFlag::eCOLLISION_WITH_DYNAMIC_ACTORS, true);
			}
			else {
				m_pParticleSystem->setParticleBaseFlag(PxParticleBaseFlag::eCOLLISION_WITH_DYNAMIC_ACTORS, false);
			}
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
		ImGui::Checkbox("random velocity range", &random_value_velocity);
		if (!random_value_velocity) {
			if (ImGui::DragFloat3("initial velocity particles", &m_initial_velocity.x, 0.1f)) {
				for (int i = 0; i < m_particles.numParticles; i++) {
					m_particles.velocityInitBuffer[i] = m_initial_velocity;
				}
			}
		}
		else {
			//random value
		}

		ImGui::Checkbox("random position range", &random_value_position);
		if (!random_value_position) {
			if (ImGui::DragFloat3("initial position particles", &m_initial_pos.x, 0.1f)) {
				for (int i = 0; i < m_particles.numParticles; i++) {
					m_particles.positionInitBuffer[i] = m_initial_pos;
				}
			}
		}
		else {
			//random value
		}

		ImGui::Checkbox("random lifeTime range", &random_value_lifeTime);
		if (!random_value_lifeTime) {
			if (ImGui::DragFloat("initial lifeTime particles", &m_initial_lifeTime, 0.1f)) {
				//changes applied when particles born again
				for (unsigned idx = 0; idx < data->validParticleRange; ++idx, ++particleFlags, ++positions, ++velocities) {
					m_particles.maxLifeTimeBuffer[idx] = m_initial_lifeTime;
				}
			}
		}
		else {
			//random value
		}

		ImGui::TreePop();
	}


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

					ImGui::TreePop();
				}
			}
		}

		ImGui::TreePop();
	}

	data->unlock();
}