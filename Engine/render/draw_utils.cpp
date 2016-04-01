#include "mcv_platform.h"
#include "draw_utils.h"
#include "render/mesh.h"
#include "resources/resources_manager.h"

CShaderCte< TCteCamera > shader_ctes_camera;
CShaderCte< TCteObject > shader_ctes_object;
CShaderCte< TCteBones >  shader_ctes_bones;

void drawLine(const VEC3& src, const VEC3& dst, const VEC4& color) {
  MAT44 world = MAT44::CreateLookAt(src, dst, VEC3(0, 1, 0)).Invert();
  float distance = VEC3::Distance(src, dst);
  world = MAT44::CreateScale(1, 1, -distance) * world;
  shader_ctes_object.activate(CTE_SHADER_OBJECT_SLOT);
  shader_ctes_object.World = world;
  //shader_ctes_object.obj_color = color;
  shader_ctes_object.uploadToGPU();
  auto line = Resources.get("line.mesh")->as<CMesh>();
  line->activateAndRender();
  //shader_ctes_object.obj_color = VEC4(1, 1, 1, 1);
}
