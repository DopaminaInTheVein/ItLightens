#include "mcv_platform.h"
#include "damage.h"

std::vector<Damage::TDamage> Damage::types = {};

using namespace std;

void Damage::init() {
	CApp &app = CApp::get();
	string file_ini = app.file_initAttr_json;
	map<string, float> fields_base = readIniAtrData(file_ini, "bt_guard");
	float DAMAGE_LASER;
	assignValueToVar(DAMAGE_LASER, fields_base);
	//Type, DamageOnce, DamagePerSecond, Cumulative
	types.resize(SIZE);
	CONFIG_DAMAGE(ABSORB, 0.f, DAMAGE_LASER, true);
	CONFIG_DAMAGE(WATER, 0.f, 10.f, false);
}

//TODO: Poder modificar estos valores en ImGUI
//(añadir Damage::renderInMenu() en update para que funcione)
//void renderInMenu() {
//
//}