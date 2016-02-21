#include "mcv_platform.h"
#include "aicontroller.h"

void aicontroller::Init()
{
}


void aicontroller::Recalc()
{
// this is a trusted jump as we've tested for coherence in ChangeState
(this->*statemap[state])();
}


void aicontroller::ChangeState(std::string newstate)
{
// try to find a state with the suitable name
if (statemap.find(newstate) == statemap.end())
	{
	// the state we wish to jump to does not exist. we abort
	exit(-1);
	}
state=newstate;
}


void aicontroller::AddState(std::string name, statehandler sh)
{
// try to find a state with the suitable name
if (statemap.find(name) != statemap.end())
	{
	// the state we wish to jump to does exist. we abort
	exit(-1);
	}
statemap[name]=sh;
}