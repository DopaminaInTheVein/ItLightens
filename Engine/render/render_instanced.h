#ifndef INC_RENDER_INSTANCED_H_
#define INC_RENDER_INSTANCED_H_

#include "mcv_platform.h"

// -------------------
struct TParticle {
  VEC3  center;
  //float size;
  //float spin;
  //float utime;
  float nframe;
};

// ------------------------------
class CRenderParticlesInstanced {

  const CMesh* instanced_mesh;  // billboard
  CMesh*       instances_data_mesh;   // VB to contain the centers for example
  
  std::vector< TParticle > instances;

  const CRenderTechnique*  tech;
  const CTexture*          texture;

  float global_time;

public:

  bool create(size_t max_instances, const CMesh* new_instanced_mesh);
  void update(float dt);

  void render() const;

};

extern CRenderParticlesInstanced render_particles_instanced;

#endif

