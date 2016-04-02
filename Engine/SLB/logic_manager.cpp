#include "mcv_platform.h"
#include "logic_manager.h"
#include "app_modules\io\io.h"

void LogicManager::bind(SLB::Manager& m) {
	SLB::Class<Character>("Character", &m)
		.comment("Character game class")
		.constructor()
		.set("getName", &Character::getName)
		.set("setName", &Character::setName)
		.comment("Method to change the character name")
		.param("New name of the character")
		.property("life", &Character::life)
		;
}

void LogicManager::run() {
	dbg("Welcome\n");
	_getch();

	SLB::Manager m;
	bind(m);

	SLB::Script s(&m);

	s.doFile("data/lua_scripts/coroutines.lua");

	char buffer[64];
	float elapsed = 1 / 60.f;

	while (true) {
		sprintf(buffer, "updateCoroutines(%f);", elapsed);
		s.doString(buffer);

		io->update(elapsed);
		if (io->keys[VK_ESCAPE].becomesPressed())
			break;
		if (io->keys['R'].becomesPressed())
			s.doFile("test.lua");

		Sleep(elapsed * 1000);
	}

	s.doFile("test.lua");
	while (true) {
		io->update(elapsed);
		if (io->keys[VK_RETURN].becomesPressed())
			break;
	}

	dbg("Bye\n");
	_getch();
}