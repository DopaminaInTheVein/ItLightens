#ifndef _AICONTROLLER_H
#define _AICONTROLLER_H

// ai controllers using maps to function pointers for easy access and scalability. 

// we put this here so you can assert from all controllers without including everywhere
#include <assert.h>	
#include <string>
#include <map>

#include "../entities/tentity.h"
#include "sbb.h"

using namespace std;

// states are a map to member function pointers, to 
// be defined on a derived class. 
class aicontroller;

typedef void (aicontroller::*statehandler)(); 

class aicontroller
	{
	string state;
	
	// the states, as maps to functions
	map<string,statehandler>statemap;

	public:
		TEntity* entity;
		TEntity* player;
		sbb* shared_board;

		void ChangeState(string);	// state we wish to go to
		virtual void Init();	// resets the controller
		void Recalc();	// recompute behaviour
		void AddState(string,statehandler);
		void setPlayer(TEntity* new_player) {
			player = new_player;
		}
		void setSbb(sbb* new_shared_board) {
			shared_board = new_shared_board;
		}

	};

#endif