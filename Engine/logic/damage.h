#ifndef INC_DAMAGE_H_
#define INC_DAMAGE_H_

#define CONFIG_DAMAGE(type, dmg, dmgPerSecond, cum) \
Damage::types[type] = TDamage(type,dmg,dmgPerSecond)

#define DMG_ONCE(type) \
Damage::types[type].damageOnce

#define DMG_PER_SECOND(type) \
Damage::types[type].damagePerSecond

#define DMG_IS_CUMULATIVE(type) \
Damage::types[type].cumulative



//--------------------------------------------------------------------
namespace Damage {
	struct TDamage {
		float damageOnce;
		float damagePerSecond;
		bool cumulative;
		TDamage(float dmgOnce, float dmgPerSecond, bool cum)
			: damageOnce(dmgOnce), damagePerSecond(dmgPerSecond), cumulative(cum) {};
		TDamage()
			: damageOnce(0), damagePerSecond(0), cumulative(false) {};

	};
	enum DMG_TYPE {
		ABSORB = 0,
		WATER,
		SIZE
	};

	static TDamage types[DMG_TYPE::SIZE];
	static void init() {
		//TODO: assignValueToVar(..., ...);
		//Type, DamageOnce, DamagePerSecond, Cumulative
		CONFIG_DAMAGE(ABSORB, 0.f, 10.f, true);
		CONFIG_DAMAGE(WATER, 0.f, 10.f, false);
	}

	//TODO: Poder modificar estos valores en ImGUI
	//(a�adir Damage::renderInMenu() en update para que funcione)
	//void renderInMenu() {
	//	
	//}
}

#endif