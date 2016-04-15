#ifndef INC_COMPONENTS_H_
#define INC_COMPONENTS_H_

#include "entity.h"
#include "comp_name.h"
#include "comp_transform.h"
#include "comp_camera.h"
#include "comp_render_static_mesh.h"
#include "comp_controller_3rd_person.h"
#include "comp_life.h"
#include "comp_tags.h"
#include "skeleton/comp_skeleton.h"
#include "skeleton/comp_bone_tracker.h"
#include "comp_victory_point.h"
#include "comp_trigger_lua.h"
#include "comp_platform.h"
#include "comp_box.h"
#include "comp_tracker.h"

//Logic
#include "logic/ai_beacon.h"
#include "logic/ai_workbench.h"
#include "logic/ai_mole.h"
#include "logic/ai_scientific.h"
#include "logic/ai_speedy.h"
#include "logic/ai_guard.h"
#include "logic/aicontroller.h"

//controllers
#include "player_controllers\player_controller.h"
#include "player_controllers/player_controller_speedy.h"
#include "player_controllers/player_controller_mole.h"
#include "player_controllers/player_controller_cientifico.h"

//Physics
#include "comp_physics.h"
#include "comp_charactercontroller.h"

//prefabs
#include "prefabs\magnetic_bomb.h"
#include "prefabs\static_bomb.h"
#include "comp_wire.h"
#include "comp_generator.h"
#include "comp_polarized.h"

#endif
