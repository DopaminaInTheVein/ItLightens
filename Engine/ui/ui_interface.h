#ifndef INC_UI_H_
#define	INC_UI_H_

#include "imgui\imgui.h"
#include "components/entity_tags.h"
#include "components/entity.h"
#include "components/components.h"

#include "input\input.h"

extern CInput Input;

class CUI {
	bool open_main_ui = true;
	bool open_ui_keys = true;
	bool open_assist = true;
	vector<std::string> msgs;

public:
	void update() {
		//Here goes all game UI info

		ImGui::Begin("UI", &open_main_ui, ImVec2(400, 150), -1.0f);

		life_update();

		keys_update();
		instructions_update();

		ImGui::End();

		ui_assist();
	}

	void ui_assist() {
		ImGui::Begin("Help", &open_assist, ImVec2(400, 150), -1.0f);
		for (auto msg : msgs) {
			ImGui::Text(msg.c_str());
		}
		ImGui::End();
		msgs.clear();
	}

	void instructions_update() {
		if (ImGui::CollapsingHeader("Instructions")) {
			ImGui::Text("Instructions goes here");
		}
	}

	void keys_update() {
		if (ImGui::CollapsingHeader("Game controls", 0, true, open_ui_keys)) {
			ImGui::Text("unlock mouse -> T");
			ImGui::Text("w,a,s,d/ L joystick -> player moving");
			ImGui::Text("mouse mov/ R joystick -> camera moving");
			ImGui::Text("SPACE/ A -> jump,doublejump(if possible)");
			ImGui::Text("1,2/ RB,LB -> Move to POSITIVE/NEGATIVE atraction Point");
		}
	}

	void life_update() {
		TTagID tagIDplayer = getID("player");
		CHandle player = tags_manager.getFirstHavingTag(tagIDplayer);
		CEntity * player_e = player;

		TCompController3rdPerson * camara3rd = player_e->get<TCompController3rdPerson>();
		CEntity * target_e = camara3rd->target;

		TCompLife * life = target_e->get<TCompLife>();

		ImGui::Text("LifeBar\n");
		std::string lifeString = "|";
		if (life) {
			for (int i = 0; i < life->maxlife; i++) {
				if (life->currentlife > i) {
					lifeString += "=";
				}
				else {
					lifeString += ".";
				}
			}
		}
		lifeString += "|";
		ImGui::Text(lifeString.c_str());
		ImGui::Separator();

		//put cheats here:
		ImGui::Text("Press 'ESC' to restart");

		//if (Input.IsKeyPressedDown(KEY_L))
			//life->refillLife();
		CEntity * victoryPoint = tags_manager.getFirstHavingTag(getID("victory_point"));
		TCompTransform * player_transform = target_e->get<TCompTransform>();
		TCompTransform * victoryPoint_transform = victoryPoint->get<TCompTransform>();

		if (life->currentlife <= 0.0f) {
			bool open = true;
			ImGui::Begin("Fatal Player State", &open, ImVec2(300, 100), -1.0f);
			ImGui::Text("You are Dead!\n");
			ImGui::Text("Press 'ESC' to restart");
			ImGui::End();
		}
		else if (0.5f > simpleDist(victoryPoint_transform->getPosition(), player_transform->getPosition())) {
			bool open = true;
			ImGui::Begin("Victory Player State", &open, ImVec2(300, 100), -1.0f);
			ImGui::Text("You WIN!\n");
			ImGui::Text("Press 'ESC' to restart");
			ImGui::End();
		}
	}

	void addTextInstructions(std::string message) {
		msgs.push_back(message.c_str());
	}
};

extern CUI ui;
#endif
