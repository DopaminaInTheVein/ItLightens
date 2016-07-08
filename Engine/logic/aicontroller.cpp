#include "mcv_platform.h"
#include "aicontroller.h"

using namespace std;

void aicontroller::Init()
{
}

void aicontroller::Recalc()
{
	// this is a trusted jump as we've tested for coherence in ChangeState
	(this->*(*getStatemap())[state])();
}

void aicontroller::ChangeState(std::string newstate)
{
	// try to find a state with the suitable name
	if (getStatemap()->find(newstate) == getStatemap()->end())
	{
		// the state we wish to jump to does not exist. we abort
		//exit(-1);
		assert(false || fatal("state doesnt exist!\n"));
	}
	state = newstate;
}

void aicontroller::AddState(std::string name, statehandler sh)
{
	// try to find a state with the suitable name
	if (getStatemap()->find(name) != getStatemap()->end())
	{
		// the state we wish to jump to does exist. we abort
		//exit(-1);
		assert(false || fatal("state doesnt exist!\n"));
	}
	(*getStatemap())[name] = sh;
}

void aicontroller::DeleteState(string name) {
	getStatemap()->erase(name);
}

map<string, statehandler>* aicontroller::getStatemap() {
	//Must implement in subclasses
	return nullptr;
}

string aicontroller::getState() {
	return state;
}