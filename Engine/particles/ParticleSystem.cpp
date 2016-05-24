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

void CParticleSystem::init() {
  m_numParticles = 1;
  m_pParticleSystem = g_PhysxManager->CreateParticleSystem(m_numParticles);

  m_pParticle_mesh = Resources.get("textured_quad_xy_centered.mesh")->as<CMesh>();

  CEntity * e = CHandle(this).getOwner();
  TCompTransform *t = e->get<TCompTransform>();
  VEC3 curr_pos = t->getPosition();

  PxVec3 physx_pos = PhysxConversion::Vec3ToPxVec3(curr_pos);

  //initial values

  PxVec3 v = PxVec3(0, 5, 0);

  m_initial_pos = physx_pos;
  m_initial_velocity = v;

  m_particles.initialize(m_numParticles);

  for (int i = 0; i < m_numParticles; i++) {
    int r = rand() % 100; //this produces numbers between -2000 - +2000
    float max_time = r / 10.0f;
    m_particles.indexBuffer[i] = i;
    m_particles.maxLifeTimeBuffer[i] = max_time; //max time
    m_particles.lifeTimeBuffer[i] = 20.0f;
    m_particles.positionBuffer[i] = m_initial_pos;
    m_particles.velocityBuffer[i] = m_initial_velocity;
    m_particles.lifeTimeBuffer[i] = 0;

    m_particles.positionInitBuffer[i] = m_initial_pos;
    m_particles.velocityInitBuffer[i] = m_initial_velocity;
  }

  m_pIndexPool = PxParticleExt::createIndexPool(10);

  bool ret = CreateParticles(m_particles);
  if (!ret) {
    fatal("particles not created\n");
    return;
  }

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
    m_numParticles = particles.numParticles;
    bool ok = m_pParticleSystem->createParticles(particleCreationData);

    m_pParticleSystem->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);   //always disabled gravity for now

    PX_UNUSED(ok);
    PX_ASSERT(ok);

    m_RenderParticles.create(m_pParticleSystem->getMaxParticles(), m_pParticle_mesh);

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
  m_numParticles = 0;

  std::vector<PxU32> mTmpIndexArray;
  PxU32 newValidRange = 0;

  if (data->validParticleRange > 0)
  {
    PxStrideIterator<const PxVec3> positions(data->positionBuffer);
    PxStrideIterator<const PxVec3> velocities(data->velocityBuffer);
    PxStrideIterator<const PxParticleFlags> particleFlags(data->flagsBuffer);
    //PxMemCopy(m_pParticleValidity, data->validParticleBitmap, ((data->validParticleRange + 31) >> 5) << 2);

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
          if (m_particles.lifeTimeBuffer[idx] <= 0.0)
          {
            m_particles.lifeTimeBuffer[idx] = m_particles.maxLifeTimeBuffer[idx];
            mTmpIndexArray.push_back(idx);
            removed = true;
          }
        }

        //if particle is not removed
        if (!removed)
        {
          m_particles.positionBuffer[idx] = positions[idx];
          m_particles.velocityBuffer[idx] = velocities.ptr() ? velocities[idx] : PxVec3(0.0f);

          //add dt to particle lifetime
          if (m_particles.maxLifeTimeBuffer[idx] > 0)
          {
            m_particles.lifeTimeBuffer[idx] -= elapsed;
          }

          //draw particle, disabled for now
          //VEC3 pos = PhysxConversion::PxVec3ToVec3(positions[idx]);
          //Debug->DrawLine(pos, forward, 2);

          m_numParticles++;
          newValidRange = idx;
        }

        //if removed mark particle as invalid
        else
        {
          int i = 0;
          //m_pParticleValidity[w] &= (b - 1);
        }
      }
    }
  }

  m_validParticleRange = newValidRange;
  data->unlock();

  //free removed particles
  if (m_numParticles > 0 && mTmpIndexArray.size() != 0)
  {
    PxStrideIterator<const PxU32> indexData(&mTmpIndexArray[0]);

    m_pParticleSystem->releaseParticles(static_cast<PxU32>(mTmpIndexArray.size()), indexData);
    m_pIndexPool->freeIndices(static_cast<PxU32>(mTmpIndexArray.size()), indexData);

    auto initPos = PxStrideIterator<const PxVec3>(&m_particles.positionInitBuffer[0]);
    auto initVel = PxStrideIterator<const PxVec3>(&m_particles.velocityInitBuffer[0]);

    m_pParticleSystem->setPositions(static_cast<PxU32>(mTmpIndexArray.size()), indexData, initPos);
    m_pParticleSystem->addForces(static_cast<PxU32>(mTmpIndexArray.size()), indexData, initVel, PxForceMode::eVELOCITY_CHANGE);
  }

  /*bool ret = CreateParticles(m_particles);
  if (!ret) {
    fatal("particles not created\n");
  }*/

  //update particle render data
  m_RenderParticles.update(elapsed, m_particles);
}

bool CParticleSystem::load(MKeyValue & atts)
{
  //default temp texture
  m_pTexture = Resources.get("textures/fire.dds")->as<CTexture>();

  return true;
}

void CParticleSystem::renderInMenu()
{
  PxParticleReadData *data = m_pParticleSystem->lockParticleReadData();

  ImGui::Text("num valid particles: %d\n", data->nbValidParticles);
  ImGui::Text("valid range: %d\n", data->validParticleRange);

  PxStrideIterator<const PxVec3> positions(data->positionBuffer);
  PxStrideIterator<const PxVec3> velocities(data->velocityBuffer);
  PxStrideIterator<const PxParticleFlags> particleFlags(data->flagsBuffer);

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

  data->unlock();
}