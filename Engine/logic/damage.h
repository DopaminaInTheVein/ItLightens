#ifndef INC_DAMAGE_H_
#define INC_DAMAGE_H_

#define CONFIG_DAMAGE(type, dmg, dmgPerSecond, cum) Damage::types[type] = TDamage(dmg, dmgPerSecond, cum)

#define DMG_ONCE(type) \
Damage::types[type].damageOnce

#define DMG_PER_SECOND(type) \
Damage::types[type].damagePerSecond

#define DMG_IS_CUMULATIVE(type) \
Damage::types[type].cumulative



//--------------------------------------------------------------------
class Damage {
public:
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
	static std::vector<TDamage> types;

	static void init();

};

#endif