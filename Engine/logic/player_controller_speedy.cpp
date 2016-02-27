#include "mcv_platform.h"
#include "player_controller_speedy.h"

#include <windows.h>
#include "handle\object_manager.h"
#include "components\comp_transform.h"
#include "components\entity.h"
#include "components\entity_tags.h"

#include "components\comp_msgs.h"


void player_controller_speedy::Init() 
{
	om = getHandleManager<player_controller_speedy>();	//player

	AddState("dashing", (statehandler)&player_controller_speedy::Dashing);		
	AddState("blinking", (statehandler)&player_controller_speedy::Blinking);
	AddState("blink", (statehandler)&player_controller_speedy::Blink);

	myHandle = om->getHandleFromObjAddr(this);
	myParent = myHandle.getOwner();
	myEntity = myParent;
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	starting_player_y = player_transform->getPosition().y + 2;
	player_y = starting_player_y;

	ChangeState("idle");
}

void player_controller_speedy::update(float elapsed) {
	Input.Frame();
	UpdateInputActions();
	Recalc();
	UpdateMoves();
	updateDashTimer();
	updateBlinkTimer();
}

void player_controller_speedy::Dashing() 
{
	if (dash_ready) {
		bool arrived = dashFront();
		if (arrived) {
			resetDashTimer();
			ChangeState("idle");			
		}		
	}
	else {
		ChangeState("idle");		
	}	
}

void player_controller_speedy::Blinking()
{
	if (blink_ready) {
		
		// TODO: Marcar punto
		if (Input.IsOrientLeftPressed() || Input.IsMouseMovedLeft())
			rotate = 1;
		else if (Input.IsOrientRightPressed() || Input.IsMouseMovedRight())
			rotate = -1;
		else
			rotate = 0;

		Input.UpdateMousePosition();

		if (Input.IsRightClickReleased())
			ChangeState("blink");
		
	}
	else {
		ChangeState("idle");		
	}
}

void player_controller_speedy::Blink()
{
	if (blink_ready) {
		SetMyEntity();
		TCompTransform* player_transform = myEntity->get<TCompTransform>();
		VEC3 player_position = player_transform->getPosition();
		VEC3 player_front = player_transform->getFront();
		
		player_position += player_front * blink_distance;
		
		player_transform->setPosition(player_position);
		
		resetBlinkTimer();		
	}	
	ChangeState("idle");
}

void player_controller_speedy::UpdateInputActions() {

	if (Input.IsLeftClickPressedDown())	
		ChangeState("dashing");
	if (Input.IsRightClickPressed())
		ChangeState("blinking");
}

bool player_controller_speedy::dashFront() 
{
	dash_duration += getDeltaTime();
	
	SetMyEntity();
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();
	VEC3 player_front = player_transform->getFront();
	
	VEC3 new_position = VEC3(player_position.x + player_front.x*dash_speed, player_position.y, player_position.z + player_front.z*dash_speed);

	player_transform->setPosition(new_position);
	
	if (dash_duration > dash_max_duration) {
		dash_duration = 0;
		return true;		
	}
	else {
		return false;		
	}
	
}

void player_controller_speedy::updateDashTimer() 
{
	dash_timer -= getDeltaTime();
	if (dash_timer <= 0) {
		dash_ready = true;		
	}	
}

void player_controller_speedy::resetDashTimer() 
{
	dash_timer = dash_cooldown;
	dash_ready = false;	
}

void player_controller_speedy::updateBlinkTimer() 
{
	blink_timer -= getDeltaTime();
	if (blink_timer <= 0) {
		blink_ready = true;		
	}	
}

void player_controller_speedy::resetBlinkTimer() 
{
	blink_timer = blink_cooldown;
	blink_ready = false;	
}