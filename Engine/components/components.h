#ifndef INC_COMPONENTS_H_
#define INC_COMPONENTS_H_

#include "entity.h"
#include "comp_name.h"
#include "comp_room.h"
#include "comp_room_switch.h"
#include "comp_room_limit.h"
#include "comp_transform.h"
#include "comp_transform_animator.h"
#include "comp_snoozer.h"
#include "comp_camera.h"
#include "comp_camera_main.h"
#include "comp_guided_camera.h"
#include "comp_render_static_mesh.h"
#include "comp_controller_3rd_person.h"
#include "comp_hierarchy.h"
#include "comp_aabb.h"
#include "comp_culling.h"
#include "comp_life.h"
#include "comp_tags.h"
#include "skeleton/comp_skeleton.h"
#include "skeleton/comp_bone_tracker.h"
#include "skeleton/comp_skeleton_ik.h"
#include "skeleton/comp_skeleton_lookat.h"
#include "skeleton_controllers/skc_player.h"
#include "skeleton_controllers/skc_guard.h"
#include "skeleton_controllers/skc_scientist.h"
#include "skeleton_controllers/skc_mole.h"
#include "comp_victory_point.h"
#include "comp_trigger_lua.h"
#include "comp_platform.h"
#include "comp_drone.h"
#include "comp_tracker.h"
#include "comp_box_spawner.h"
#include "comp_box_destructor.h"

#include "comp_render_glow.h"

//lights
#include "comp_light_dir.h"
#include "comp_light_dir_shadows.h"
#include "comp_light_dir_shadows_dyn.h"
#include "comp_light_point.h"
#include "comp_light_fadable.h"

//Logic
#include "logic/ai_cam.h"
#include "logic/ai_workbench.h"
#include "logic/aicontroller.h"
#include "logic/magnet_door.h"
#include "logic/elevator.h"
#include "logic/workbench.h"
#include "logic/comp_box.h"
#include "logic/comp_box_placer.h"
#include "logic/pila.h"
#include "logic/pila_container.h"

//helpers
#include "logic/helper_arrow.h"
#include "components/comp_fading_message.h"
#include "components/comp_text.h"
#include "components/comp_fading_globe.h"
#include "components/comp_loading_screen.h"
#include "components/comp_look_target.h"

#include "comp_trigger_standar.h"

//controllers
#include "player_controllers/player_controller.h"
#include "player_controllers/player_controller_mole.h"
#include "player_controllers/player_controller_cientifico.h"

//Physics
#include "comp_physics.h"
#include "comp_charactercontroller.h"
#include "particles/ParticlesSystem.h"

//prefabs
#include "prefabs/magnetic_bomb.h"
#include "prefabs/static_bomb.h"
#include "prefabs/throw_bomb.h"
#include "comp_wire.h"
#include "comp_generator.h"
#include "comp_polarized.h"

//FX
#include "comp_render_fade_screen.h"
#include "comp_sense_vision.h"

//Sound
#include "app_modules/sound_manager/comp_sound.h"

//GUI
#include "app_modules/gui/comps/gui_basic.h"
#include "app_modules/gui/comps/gui_cursor.h"
#include "app_modules/gui/comps/gui_button.h"
#include "app_modules/gui/comps/gui_selector.h"
#include "app_modules/gui/comps/gui_drag.h"

#endif
