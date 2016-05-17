#include "mcv_platform.h"
#include "render/render.h"
#include "render/render_instanced.h"
#include "render/draw_utils.h"
#include "render/mesh.h"
#include "resources/resources_manager.h"

CRenderParticlesInstanced render_particles_instanced;

float urandom() {
  return (float)rand() / (float)RAND_MAX;
}

float random(float vmin, float vmax) {
  return vmin + urandom() * (vmax - vmin);
}

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
    //p.size = 1.f;
    //p.spin = 0.f;
    //p.utime = 0.f;
    p.nframe = random(0, 15);
    ++idx;
  }

  // This mesh has not been registered in the mesh manager
  instances_data_mesh = new CMesh("instanced_particles");
  bool is_ok = instances_data_mesh->create( 
      n
    , sizeof( TParticle )
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
  assert(instanced_mesh);
  activateWorldMatrix(MAT44::Identity);
  tech->activate();
  texture->activate(TEXTURE_SLOT_DIFFUSE);
  activateBlend(BLENDCFG_COMBINATIVE);
  activateZ(ZCFG_TEST_BUT_NO_WRITE);
  instanced_mesh->renderInstanced(instances_data_mesh, instances.size());
  activateZ(ZCFG_DEFAULT);
  activateBlend(BLENDCFG_DEFAULT);
}

void CRenderParticlesInstanced::update(float elapsed) {

  // Update particles using some cpu code
  global_time += elapsed;
  int idx = 1;
  for (auto& p : instances) {
    p.nframe += elapsed;
    /*
    p.center.y -= random(1.f, 3.f) * elapsed* 2;
    if (p.center.y < 0)
      p.center.y += 50.f;
      */
    ++idx;
  }

  instances_data_mesh->updateFromCPU(&instances[0]);

}