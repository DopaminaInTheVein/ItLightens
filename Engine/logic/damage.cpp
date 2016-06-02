#include "mcv_platform.h"
#include "damage.h"

std::vector<Damage::TDamage> Damage::types = {};

void Damage::init() {
	//TODO: assignValueToVar(..., ...);
	//Type, DamageOnce, DamagePerSecond, Cumulative
	types.resize(SIZE);
	CONFIG_DAMAGE(ABSORB, 0.f, 20.f, true);
	CONFIG_DAMAGE(WATER, 0.f, 10.f, false);
}

//TODO: Poder modificar estos valores en ImGUI
//(añadir Damage::renderInMenu() en update para que funcione)
//void renderInMenu() {
//	
//}