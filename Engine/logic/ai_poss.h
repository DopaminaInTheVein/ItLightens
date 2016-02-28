#ifndef INC_AI_POSS_H_
#define INC_AI_POSS_H_

#include "aicontroller.h"
#include "components/comp_msgs.h"
#include "components/entity.h"
#include <string>

#define ST_POSSESSING	"possessionStart"
#define ST_POSSESSED	"possessed"
#define ST_UNPOSSESSING	"possessionEnd"

#define ST_STUNT			"stunt"
#define ST_STUNT_END		"stuntEnd"

enum ACTION_RESULT {
	DONE
	, IN_PROGRESS
	, CANCEL	//No creo que lo usemos
};

class ai_poss : public aicontroller {
private:
	bool stunned;
	bool possessed;
public:
	ai_poss();

	____TIMER_DECLARE_(timeStunt, 15.0f);
	const void PossessingState();
	const void PossessedState();
	const void UnpossessingState();
	const void StuntState();
	virtual void actionStunt();

	//Preguntar por Stunt o Poseido
	bool isStunned() { return stunned; }
	bool isPossessed() { return possessed; }
	bool isAvailable() { return !stunned && !possessed; }

	//Estado cuando el bot se recupera del stunt
	virtual void _StuntEndState();
	virtual void idle() {}

	//Funciones que utiliza para poder cambiar entre los estados  anteriores

	virtual ACTION_RESULT _actionBeingPossessed();
	virtual ACTION_RESULT _actionBeingUnpossessed();

	//Accion que se ejecuta antes de empezar el proceso de posesión
	//(útil para guardarse último estado antes de ser poseído, etc.)
	virtual void _actionBeforePossession();

	//Avisar a la entidad que ha sido poseída
	void onSetPossessed(const TMsgAISetPossessed&);

	//TComponent
	virtual CEntity* getMyEntity() = 0; //Pasar al TCompBase?

	//FUNCIONES QUE DEFINIR EN HIJOS (Copy & Paste):
	//virtual void _actionBeforePossession();
	//virtual ACTION_RESULT _actionBeingPossessed();
	//virtual ACTION_RESULT _actionBeingUnpossessed();
	//virtual void _actionStunt();
	//virtual void _StuntEndState();
	//virtual CEntity* getMyEntity() = 0;
};

#endif