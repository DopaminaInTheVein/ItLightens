#include "mcv_platform.h"
#include "lights_data.h"

#include "resources\resources_manager.h"
#include "render\draw_utils.h"

void TContainerCubemaps::uploadTextures()
{
	//hub
	Resources.get("textures/environment/hub/hub_env_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_HUB1);

	//tuto scientist cubemaps
	Resources.get("textures/environment/sci_room/tsci_env_1_3_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_SCI3);
	Resources.get("textures/environment/sci_room/tsci_env_1sup_3_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_SCI4);
	Resources.get("textures/environment/sci_room/tsci_env_2_3_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_SCI2);
	Resources.get("textures/environment/sci_room/tsci_env_3_3_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_SCI1);

	//tuto mole
	Resources.get("textures/environment/mole_room/tmole_env_1_no_wall_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_MOLE1);
	Resources.get("textures/environment/mole_room/tmole_env_2_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_MOLE2);
	Resources.get("textures/environment/mole_room/tmole_env_3_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_MOLE3);
	Resources.get("textures/environment/mole_room/tmole_env_4_jump_area_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_MOLE4);

	//ms3 - room1
	/*Resources.get("textures/environment/ms3_room/ms3_1a_S_nv1_env_22_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_1_01);
	Resources.get("textures/environment/ms3_room/ms3_1a_S_nv1_env_20_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_1_02);
	Resources.get("textures/environment/ms3_room/ms3_1a_S_nv1_env_18_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_1_03);
	Resources.get("textures/environment/ms3_room/ms3_1a_S_nv1_env_21_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_1_04);
	Resources.get("textures/environment/ms3_room/ms3_1a_S_nv1_env_19_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_1_05);
	Resources.get("textures/environment/ms3_room/ms3_1a_S_nv1_env_15_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_1_06);
	Resources.get("textures/environment/ms3_room/ms3_1a_S_nv1_env_13_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_1_07);
	/*Resources.get("textures/environment/ms3_room/ms3_1a_S_nv1_env_14_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_1_08);
	Resources.get("textures/environment/ms3_room/ms3_1a_S_nv1_env_11_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_1_09);
	Resources.get("textures/environment/ms3_room/ms3_1a_S_nv1_env_12_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_1_10);
	Resources.get("textures/environment/ms3_room/ms3_1a_S_nv1_env_16_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_1_11);
	Resources.get("textures/environment/ms3_room/ms3_1a_S_nv1_env_17_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_1_12);
	Resources.get("textures/environment/ms3_room/ms3_1a_S_nv1_env_07_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_1_13);
	Resources.get("textures/environment/ms3_room/ms3_1a_S_nv1_env_09_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_1_14);
	Resources.get("textures/environment/ms3_room/ms3_1a_S_nv1_env_08_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_1_15);
	Resources.get("textures/environment/ms3_room/ms3_1a_S_nv1_env_04_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_1_16);
	Resources.get("textures/environment/ms3_room/ms3_1a_S_nv1_env_05_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_1_17);
	Resources.get("textures/environment/ms3_room/ms3_1a_S_nv1_env_06_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_1_18);
	Resources.get("textures/environment/ms3_room/ms3_1a_S_nv1_env_01_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_1_19);
	Resources.get("textures/environment/ms3_room/ms3_1a_S_nv1_env_03_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_1_20);
	Resources.get("textures/environment/ms3_room/ms3_1a_S_nv1_env_02_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_1_21);
	Resources.get("textures/environment/ms3_room/ms3_1a_S_nv1_env_10_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_1_22);
	Resources.get("textures/environment/ms3_room/ms3_1a_S_nv2_env_1_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_1_23);
	Resources.get("textures/environment/ms3_room/ms3_1a_S_nv2_env_2_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_1_24);
	Resources.get("textures/environment/ms3_room/ms3_1a_S_nv2_env_3_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_1_25);
	Resources.get("textures/environment/ms3_room/ms3_1a_S_nv2_env_4_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_1_26);
	Resources.get("textures/environment/ms3_room/ms3_1a_S_nv2_env_5_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_1_27);
												 
	//ms3 - room2								 
	Resources.get("textures/environment/ms3_room/ms3_2a_S_env_01_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_2_01);
	Resources.get("textures/environment/ms3_room/ms3_2a_S_env_02_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_2_02);
	Resources.get("textures/environment/ms3_room/ms3_2a_S_env_04_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_2_03);
	Resources.get("textures/environment/ms3_room/ms3_2a_S_env_06_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_2_04);
	Resources.get("textures/environment/ms3_room/ms3_2a_S_env_05_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_2_05);
	Resources.get("textures/environment/ms3_room/ms3_2a_S_env_03_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_2_06);
	Resources.get("textures/environment/ms3_room/ms3_2a_S_env_07_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_2_07);
	Resources.get("textures/environment/ms3_room/ms3_2a_S_env_08_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_2_08);
	Resources.get("textures/environment/ms3_room/ms3_2a_S_env_09_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_2_09);
	Resources.get("textures/environment/ms3_room/ms3_2a_S_env_10_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_2_10);
	Resources.get("textures/environment/ms3_room/ms3_2a_S_env_11_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_2_11);
	Resources.get("textures/environment/ms3_room/ms3_2a_S_env_12_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_2_12);
	Resources.get("textures/environment/ms3_room/ms3_2a_S_env_13_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_2_13);
	Resources.get("textures/environment/ms3_room/ms3_2a_S_ascensor_env_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_2_14);
	Resources.get("textures/environment/ms3_room/ms3_2a_S_lab_cam_1_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_2_15);
	Resources.get("textures/environment/ms3_room/ms3_2a_S_lab_cam_2_cubemap.dds")->as<CTexture>()->activate(CUBEMAP_SLOT_ROOM_2_16);
*/
}
