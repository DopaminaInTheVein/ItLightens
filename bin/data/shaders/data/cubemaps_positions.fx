#include "globals.fx"

#define hub_cubemaps 1
#define mole_cubemaps 4
#define sci_cubemaps 4

//hub cubemaps positions
static float3 cubemap_hub_1 = float3(28.732, -22.7103, -0.0930618);

//mole cubemaps positions
static float3 cubemap_mole_1 = float3(1037.67, 65.6709, -26.0246);
static float3 cubemap_mole_2 = float3(1030.04, 65.6709, -24.9064);
static float3 cubemap_mole_3 = float3(1021.16, 65.6709, -24.9064);
static float3 cubemap_mole_4 = float3(1023.47, 69.3074, -24.9064);

//scientist cubemaps positions
static float3 cubemap_sci_3 = float3(69.2549, 1.26306, -2.85067);
static float3 cubemap_sci_2 = float3(58.0463, 1.26306, -2.79071);
static float3 cubemap_sci_4 = float3(69.2549, 8.66815, -2.85067);
static float3 cubemap_sci_1 = float3(49.4013, 1.26305, -2.79072);
/*
//ms3 room 1 positions
static float3 cubemap_ms3r1_01 = float3(28.732   ,-22.7103 ,-0.0930618);
static float3 cubemap_ms3r1_02 = float3(12.655   ,-22.7103 ,15.1624);
static float3 cubemap_ms3r1_03 = float3(9.25714  ,-22.7103 ,-22.789);
static float3 cubemap_ms3r1_04 = float3(9.99446  ,-22.7103 ,21.6179 );
static float3 cubemap_ms3r1_05 = float3(12.1868  ,-22.7103 ,-6.0424 );
static float3 cubemap_ms3r1_06 = float3(2.37174  ,-22.7103 ,3.73792 );
static float3 cubemap_ms3r1_07 = float3(0.798018 ,-22.7103 ,-14.5029);
static float3 cubemap_ms3r1_08 = float3(-3.11062 ,-22.7103 ,-4.26553);
static float3 cubemap_ms3r1_09 = float3(-14.9796 ,-22.7103 ,-1.3559 );
static float3 cubemap_ms3r1_10 = float3(-9.15828 ,-22.7103 ,6.83469 );
static float3 cubemap_ms3r1_11 = float3(-2.38801 ,-22.7103 ,9.9069 );
static float3 cubemap_ms3r1_12 = float3(-0.614067, -22.7103, 21.2118);
static float3 cubemap_ms3r1_13 = float3(-26.5885 ,-22.7103 ,21.7086 );
static float3 cubemap_ms3r1_14 = float3(-18.9517 ,-22.7103 ,11.8885 );
static float3 cubemap_ms3r1_15 = float3(-18.1617 ,-22.7103 ,-9.5124);
static float3 cubemap_ms3r1_16 = float3(-19.9159 ,-22.7103 ,-22.7511);
static float3 cubemap_ms3r1_17 = float3(-23.1993 ,-22.7103 ,-20.5379);
static float3 cubemap_ms3r1_18 = float3(-24.3986 ,-22.7103 ,-1.68156);
static float3 cubemap_ms3r1_19 = float3(-31.6074 ,-25.9676 ,-4.79774);
static float3 cubemap_ms3r1_20 = float3(-33.0165 ,-25.9676 ,8.20368 );
static float3 cubemap_ms3r1_21 = float3(-31.6074 ,-18.5737 ,0.352438);
static float3 cubemap_ms3r1_22 = float3(-12.8479 ,-22.7103 ,-17.0288);
static float3 cubemap_ms3r1_23 = float3(10.2618  ,-10.6357 ,-22.4647); 
static float3 cubemap_ms3r1_24 = float3(-10.3358 ,-10.6357 ,-0.0514817);
static float3 cubemap_ms3r1_25 = float3(10.2618  ,-10.6357 ,21.9096);
static float3 cubemap_ms3r1_26 = float3(-17.7299 ,-1.24868  ,21.9148 );
static float3 cubemap_ms3r1_27 = float3(-35.4102 ,-0.101621 , -0.992577);
									
//ms3 room 2 positions                  
static float3 cubemap_ms3r2_01 = float3(-63.9483, 0.21245 , -6.91489);			
static float3 cubemap_ms3r2_02 = float3(-70.7734, 0.21245 , -6.91489);          
static float3 cubemap_ms3r2_03 = float3(-81.3564, 0.21245 , -14.4112);          
static float3 cubemap_ms3r2_04 = float3(-83.8812, 0.21245 , 2.18812);           
static float3 cubemap_ms3r2_05 = float3(-81.3341, 0.21245 , -7.88866);          
static float3 cubemap_ms3r2_06 = float3(-83.1323, 0.212449,  -26.425);           
static float3 cubemap_ms3r2_07 = float3(-92.0132, 0.21245 , -11.9543);          
static float3 cubemap_ms3r2_08 = float3(-99.6814, 0.21245 , -10.1578);          
static float3 cubemap_ms3r2_09 = float3(-112.222, 0.21245 , -10.1578);          
static float3 cubemap_ms3r2_10 = float3(-121.353, 0.21245 , -9.5728);           
static float3 cubemap_ms3r2_11 = float3(-86.6772, 0.212451,  9.88402);          
static float3 cubemap_ms3r2_12 = float3(-90.2704, 0.212451,  18.6271);          
static float3 cubemap_ms3r2_13 = float3(-67.3082, 0.212451,  12.3955);          
static float3 cubemap_ms3r2_14 = float3(-65.4052, 17.0077, 20.9509);            
static float3 cubemap_ms3r2_15 = float3(-65.4052, 17.0077, 17.8539);            
static float3 cubemap_ms3r2_16 = float3(-65.4052, 17.0077, 13.1363);            
*/
float getDistance(float3 point1, float3 point2){
	float3 vec = point1 - point2;
	return length(vec);
}

float3 GetEnvColor(float3 E_refl, float3 pixel_pos){
	float3 env = float3(0,0,0);
	//return pixel_pos.xxx;
	//return txMoleEnv2.Sample(samClampLinear, E_refl).xyz ;
	//return txHubEnv1.Sample(samClampLinear, E_refl).xyz;
	//float diff = getDistance(pixel_pos, cubemap_sci_1);
	//return diff.xxx;
	//return E_refl;
	//hub
	if(level == 1){
		//for(int i=1; i<=hub_cubemaps;i++){
			//int value = i;
			env+= txHubEnv1.Sample(samClampLinear, E_refl).xyz / distance(cubemap_hub_1, pixel_pos);
			env *= 25;
			//return float3(1,1,1);
		//}
	}
	
	//mole
	else if(level == 2){
		//return float3(1,0,1);
		env+= txMoleEnv1.Sample(samClampLinear, E_refl).xyz / distance(cubemap_mole_1, pixel_pos);
		env+= txMoleEnv2.Sample(samClampLinear, E_refl).xyz / distance(cubemap_mole_2, pixel_pos);
		env+= txMoleEnv3.Sample(samClampLinear, E_refl).xyz / distance(cubemap_mole_3, pixel_pos);
		env+= txMoleEnv4.Sample(samClampLinear, E_refl).xyz / distance(cubemap_mole_4, pixel_pos);
		//return float4(1,1,0,1);
		env /= 4;
	}
	
	//sci
	else if(level == 3){
		//return float3(0,0,1);
		env+= txSciEnv1.Sample(samClampLinear, E_refl).xyz / distance(cubemap_sci_1, pixel_pos);
		env+= txSciEnv2.Sample(samClampLinear, E_refl).xyz / distance(cubemap_sci_2, pixel_pos);
		env+= txSciEnv3.Sample(samClampLinear, E_refl).xyz / distance(cubemap_sci_3, pixel_pos);
		env+= txSciEnv4.Sample(samClampLinear, E_refl).xyz / distance(cubemap_sci_4, pixel_pos);
		//return float4(1,1,0,1);
		env /= 5;
	}
	
	//ms3 - room1
	else if(level == 4){
		/*for(int i=0; i<sci_cubemaps;i++){
			env+= float3(0,0,0);
		}*/
		/*env+= txHubEnv1.Sample(samClampLinear, E_refl).xyz;
		env+= txHubEnv1.Sample(samClampLinear, E_refl).xyz;
		env+= txHubEnv1.Sample(samClampLinear, E_refl).xyz;
		env+= txHubEnv1.Sample(samClampLinear, E_refl).xyz;*/
	}
	
	//ms3 - room1
	else if(level == 4){
		/*for(int i=0; i<sci_cubemaps;i++){
			env+= float3(0,0,0);
		}*/
		/*env+= txHubEnv1.Sample(samClampLinear, E_refl).xyz;
		env+= txHubEnv1.Sample(samClampLinear, E_refl).xyz;
		env+= txHubEnv1.Sample(samClampLinear, E_refl).xyz;
		env+= txHubEnv1.Sample(samClampLinear, E_refl).xyz;*/
	}
	
	return env;
}

//ms3 cubemaps positions
/*
<tags/><name name="1a_S_nv1_env_cam_22"/><transform pos="28.732 -22.7103 -0.0930618"
<tags/><name name="1a_S_nv1_env_cam_20"/><transform pos="12.655 -22.7103 15.1624"
<tags/><name name="1a_S_nv1_env_cam_18"/><transform pos="9.25714 -22.7103 -22.789"
<tags/><name name="1a_S_nv1_env_cam_21"/><transform pos="9.99446 -22.7103 21.6179" 
<tags/><name name="1a_S_nv1_env_cam_19"/><transform pos="12.1868 -22.7103 -6.0424" 
<tags/><name name="1a_S_nv1_env_cam_15"/><transform pos="2.37174 -22.7103 3.73792" 
<tags/><name name="1a_S_nv1_env_cam_13"/><transform pos="0.798018 -22.7103 -14.5029" 
<tags/><name name="1a_S_nv1_env_cam_14"/><transform pos="-3.11062 -22.7103 -4.26553" 
<tags/><name name="1a_S_nv1_env_cam_11"/><transform pos="-14.9796 -22.7103 -1.3559" 
<tags/><name name="1a_S_nv1_env_cam_12"/><transform pos="-9.15828 -22.7103 6.83469" 
<tags/><name name="1a_S_nv1_env_cam_16"/><transform pos="-2.38801 -22.7103 9.9069" 
<tags/><name name="1a_S_nv1_env_cam_17"/><transform pos="-0.614067 -22.7103 21.2118" 
<tags/><name name="1a_S_nv1_env_cam_07"/><transform pos="-26.5885 -22.7103 21.7086" 
<tags/><name name="1a_S_nv1_env_cam_09"/><transform pos="-18.9517 -22.7103 11.8885" 
<tags/><name name="1a_S_nv1_env_cam_08"/><transform pos="-18.1617 -22.7103 -9.5124"
<tags/><name name="1a_S_nv1_env_cam_04"/><transform pos="-19.9159 -22.7103 -22.7511"
<tags/><name name="1a_S_nv1_env_cam_05"/><transform pos="-23.1993 -22.7103 -20.5379" 
<tags/><name name="1a_S_nv1_env_cam_06"/><transform pos="-24.3986 -22.7103 -1.68156" 
<tags/><name name="1a_S_nv1_env_cam_01"/><transform pos="-31.6074 -25.9676 -4.79774" 
<tags/><name name="1a_S_nv1_env_cam_03"/><transform pos="-33.0165 -25.9676 8.20368" 
<tags/><name name="1a_S_nv1_env_cam_02"/><transform pos="-31.6074 -18.5737 0.352438" 
<tags/><name name="1a_S_nv1_env_cam_10"/><transform pos="-12.8479 -22.7103 -17.0288" 
<tags/><name name="1a_S_nv2_env_cam_1"/><transform pos="10.2618 -10.6357 -22.4647" 
<tags/><name name="1a_S_nv2_env_cam_2"/><transform pos="-10.3358 -10.6357 -0.0514817" 
<tags/><name name="1a_S_nv2_env_cam_3"/><transform pos="10.2618 -10.6357 21.9096" 
<tags/><name name="1a_S_nv2_env_cam_4"/><transform pos="-17.7299 -1.24868 21.9148" 
<tags/><name name="1a_S_nv2_env_cam_5"/><transform pos="-35.4102 -0.101621 -0.992577" 

<tags/><name name="2a_S_env_cam_01"/><transform pos="-63.9483 0.21245 -6.91489" 
<tags/><name name="2a_S_env_cam_02"/><transform pos="-70.7734 0.21245 -6.91489" 
<tags/><name name="2a_S_env_cam_04"/><transform pos="-81.3564 0.21245 -14.4112" 
<tags/><name name="2a_S_env_cam_06"/><transform pos="-83.8812 0.21245 2.18812" 
<tags/><name name="2a_S_env_cam_05"/><transform pos="-81.3341 0.21245 -7.88866"
<tags/><name name="2a_S_env_cam_03"/><transform pos="-83.1323 0.212449 -26.4251" 
<tags/><name name="2a_S_env_cam_07"/><transform pos="-92.0132 0.21245 -11.9543" 
<tags/><name name="2a_S_env_cam_08"/><transform pos="-99.6814 0.21245 -10.1578"
<tags/><name name="2a_S_env_cam_09"/><transform pos="-112.222 0.21245 -10.1578" 
<tags/><name name="2a_S_env_cam_10"/><transform pos="-121.353 0.21245 -9.5728" 
<tags/><name name="2a_S_env_cam_11"/><transform pos="-86.6772 0.212451 9.88402" 
<tags/><name name="2a_S_env_cam_12"/><transform pos="-90.2704 0.212451 18.6271" 
<tags/><name name="2a_S_env_cam_13"/><transform pos="-67.3082 0.212451 12.3955"
<tags/><name name="2a_S_ascensor_env_cam"/><transform pos="-65.4052 17.0077 20.9509" 
<tags/><name name="2a_S_lab_env_cam_1"/><transform pos="-65.4052 17.0077 17.8539" 
<tags/><name name="2a_S_lab_env_cam_2"/><transform pos="-65.4052 17.0077 13.1363" 

*/