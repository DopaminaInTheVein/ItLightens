#include "mcv_platform.h"
#include "render/render.h"
#include "render/render_instanced.h"
#include "render/draw_utils.h"
#include "render/mesh.h"
#include "resources/resources_manager.h"
#include "particles\ParticleData.h"

CRenderParticlesInstanced render_particles_instanced;
/*
float urandom() {
  return (float)rand() / (float)RAND_MAX;
}

float random(float vmin, float vmax) {
  return vmin + urandom() * (vmax - vmin);
}*/

bool CRenderParticlesInstanced::create(size_t n, const CMesh* instanced) {
  global_time = 0.f;

  tech = Resources.get("particles.tech")->as<CRenderTechnique>();;
  texture = Resources.get("textures/fire.dds")->as<CTexture>();;

  instanced_mesh = instanced;

  instances.resize(n);
  int idx = 0;
  for (auto& p : instances) {
    //p.center = VEC3(0, 0, 0);
    p.center = VEC3(random(-50, 50), 0, random(-50, 50));
    p.center.y = random(1.f, 20.f);
    p.size = 1.0f;
    p.rotation = VEC3(random(-50, 50), 0, random(-50, 50));
    //p.utime = 0.f;
    p.nframe = random(0, 15);
    p.alpha = 1.0f;
    ++idx;
  }

  // This mesh has not been registered in the mesh manager
  instances_data_mesh = new CMesh("instanced_particles");
  if (!instances_data_mesh)
    return false;
  bool is_ok = instances_data_mesh->create(
    n
    , sizeof(TParticle)
    , &instances[0]
    , 0, 0, nullptr    // No indices
    , CMesh::VTX_DECL_INSTANCED_DATA  // Type of vertex
    , CMesh::POINT_LIST     // We are not using this
    , nullptr     // Use default group information
    , true        // the buffer IS dynamic
    );

  return is_ok;
}

void CRenderParticlesInstanced::render() const {
  CTraceScoped scope("CRenderParticlesInstanced");
  if (!instanced_mesh)
    return;	//mesh can be cleared by others

  activateWorldMatrix(MAT44::Identity);
  //tech->activate();
  //texture->activate(TEXTURE_SLOT_DIFFUSE);
  activateBlend(BLENDCFG_COMBINATIVE);
  activateZ(ZCFG_TEST_BUT_NO_WRITE);
  //activateZ(ZCFG_DEFAULT);
  instanced_mesh->renderInstanced(instances_data_mesh, instances.size());
  activateZ(ZCFG_DEFAULT);
  activateBlend(BLENDCFG_DEFAULT);
}

void CRenderParticlesInstanced::update(float elapsed, const TParticleData& particle_data, float size, float modifier_size) {
  // Update particles using some cpu code
  global_time += elapsed;
  int idx = 1;

  for (auto& p : instances) {
    p.nframe += elapsed;

    /*p.center.y -= random(1.f, 3.f) * elapsed* 2;
    if (p.center.y < 0)
      p.center.y += 10.f;*/

    p.center = PhysxConversion::PxVec3ToVec3(particle_data.positionBuffer[idx - 1]);
	float modifier_over_lifetime;
	if (modifier_size < 0)
		modifier_over_lifetime = 1;
	else
        modifier_over_lifetime = (particle_data.maxLifeTimeBuffer[idx - 1] - particle_data.lifeTimeBuffer[idx - 1]) * modifier_size;

    //if lifetime expired, alpha = 0 to not render this particle
    if (particle_data.lifeTimeBuffer[idx - 1] <= 0.f)
      p.alpha = 0.f;

    p.rotation += VEC3(1, 0, 0)*elapsed;
    p.size = size*modifier_over_lifetime;

    ++idx;
  }

  if (instances_data_mesh) instances_data_mesh->updateFromCPU(&instances[0]);
}

void CRenderParticlesInstanced::clear()
{
  instances_data_mesh->destroy();
}