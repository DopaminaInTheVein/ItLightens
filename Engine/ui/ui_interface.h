#ifndef INC_UI_H_
#define	INC_UI_H_

#include "imgui\imgui.h"
#include "app_modules\gui\gui.h"
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
#ifndef NDEBUG
		ImGui::Begin("UI", &open_main_ui, ImVec2(500, 350), -1.0f);

		life_update();

		keys_update();
		instructions_update();

		ImGui::End();
		ui_assist();
#endif
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
			ImGui::Text("unlock mouse          -> º");
			ImGui::Text("w,a,s,d/ L joystick   -> player moving");
			ImGui::Text("mouse mov/ R joystick -> camera moving");
			ImGui::Text("SPACE/ A              -> jump,doublejump(if possible)");
			ImGui::Text("1,2/ RB,LB            -> Move to POSITIVE/NEGATIVE atraction Point");
			ImGui::Text("Click Left Mouse      -> Stun near IA / Recharge Energy from Recover Point");
			ImGui::Text("Left Shift            -> Possess near IA");
		}
	}

	void life_update() {
		TTagID tagIDCamera = getID("camera_main");
		ClHandle camera_h = tags_manager.getFirstHavingTag("camera_main");
		if (!camera_h.isValid()) return;
		CEntity * camera_e = camera_h;
		if (!camera_e) return;

		TCompController3rdPerson * camara3rd = camera_e->get<TCompController3rdPerson>();
		CEntity * target_e = camara3rd->target;

		if (target_e) {
			CEntity * raijin_e = tags_manager.getFirstHavingTag("raijin");
			TCompLife * life = raijin_e->get<TCompLife>();
			ImGui::Text("LifeBar\n");
			std::string lifeString = "|";
			//int increment = life->maxlife / 60;
			if (life) {
				for (int i = 0; i < life->maxlife; i++) {//= increment) {
					if (life->currentlife > i) {
						lifeString += "=";
					}
					else {
						lifeString += ".";
					}
				}
			}
			lifeString += "|\n";
			ImGui::Text(lifeString.c_str());
			ImGui::Text("%f / %f", life->currentlife, life->maxlife);
			ImGui::Separator();

			//Polarity
			player_controller * playerController = target_e->get<player_controller>();
			if (playerController) {
				ImGui::Text(playerController->GetPolarity().c_str());
				ImGui::Separator();
			}
		}
		//put cheats here:
		ImGui::Text("Press 'ESC' to exit\n");
		ImGui::Text("Press 'ENTER' to restart\n");

		//if (Input.IsKeyPressedDown(KEY_L))
			//life->refillLife();

		if (GameController->GetGameState() == CGameController::LOSE) {
			bool open = true;
			ImGui::Begin("Fatal Player State", &open, ImVec2(300, 100), -1.0f);
			ImGui::Text("You are Dead!\n");
			ImGui::Text("Press 'ESC' to exit\n");
			ImGui::Text("Press 'ENTER' to restart\n");
			ImGui::End();
		}
		else if (GameController->GetGameState() == CGameController::VICTORY) {
			bool open = true;
			ImGui::Begin("Victory Player State", &open, ImVec2(300, 100), -1.0f);
			ImGui::Text("You WIN!\n");
			ImGui::Text("Press 'ESC' to exit\n");
			ImGui::Text("Press 'ENTER' to restart\n");
			ImGui::End();
		}
	}

	void addTextInstructions(std::string message) {
		msgs.push_back(message.c_str());
	}
};

extern CUI ui;
#endif
