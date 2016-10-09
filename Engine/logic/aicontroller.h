#ifndef _AICONTROLLER_H
#define _AICONTROLLER_H

// ai controllers using maps to function pointers for easy access and scalability.

// we put this here so you can assert from all controllers without including everywhere
#include <assert.h>
#include <string>
#include <map>

// states are a map to member function pointers, to
// be defined on a derived class.
class aicontroller;

// puntero a función miembro de aicontroller
typedef void (aicontroller::*statehandler)();

using namespace std;

class aicontroller
{
protected:
	std::string state;

	virtual std::map<std::string, statehandler>* getStatemap();

public:
	virtual void ChangeState(std::string);	// state we wish to go to
	virtual void Init();	// resets the controller
	void Recalc();	// recompute behaviour
	void AddState(std::string, statehandler);
	void DeleteState(std::string);
	std::string getState();
};

#endif