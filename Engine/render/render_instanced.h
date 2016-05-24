#ifndef INC_RENDER_INSTANCED_H_
#define INC_RENDER_INSTANCED_H_

#include "mcv_platform.h"

// -------------------
struct TParticle {

  //POSITION1
  VEC3  center;

  //TEXCOORD1
  float nframe;
  float size;
  //float utime;

  //rotation
  VEC3 rotation;
  
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

