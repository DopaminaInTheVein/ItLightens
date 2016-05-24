#ifndef INC_COMPONENTS_H_
#define INC_COMPONENTS_H_

#include "entity.h"
#include "comp_name.h"
#include "comp_transform.h"
#include "comp_camera.h"
#include "comp_render_static_mesh.h"
#include "comp_controller_3rd_person.h"
#include "comp_aabb.h"
#include "comp_culling.h"
#include "comp_life.h"
#include "comp_tags.h"
#include "skeleton/comp_skeleton.h"
#include "skeleton/comp_bone_tracker.h"
#include "comp_victory_point.h"
#include "comp_trigger_lua.h"
#include "comp_platform.h"
#include "comp_drone.h"
#include "comp_box.h"
#include "comp_tracker.h"
#include "comp_box_spawner.h"
#include "comp_box_destructor.h"
#include "comp_guided_camera.h"
#include "comp_hierarchy.h"

//lights
#include "comp_light_dir.h"
#include "comp_light_dir_shadows.h"
#include "comp_light_point.h"
#include "comp_light_fadable.h"

//Logic
#include "logic/ai_beacon.h"
#include "logic/ai_workbench.h"
#include "logic/ai_mole.h"
#include "logic/ai_scientific.h"
#include "logic/ai_speedy.h"
#include "logic/ai_guard.h"
#include "logic/aicontroller.h"
#include "logic/magnet_door.h"

#include "comp_trigger_standar.h"

//controllers
#include "player_controllers\player_controller.h"
#include "player_controllers/player_controller_speedy.h"
#include "player_controllers/player_controller_mole.h"
#include "player_controllers/player_controller_cientifico.h"

//Physics
#include "comp_physics.h"
#include "comp_charactercontroller.h"
#include "particles\ParticlesSystem.h"

//prefabs
#include "prefabs\magnetic_bomb.h"
#include "prefabs\static_bomb.h"
#include "comp_wire.h"
#include "comp_generator.h"
#include "comp_polarized.h"

#endif
