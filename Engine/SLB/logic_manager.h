#ifndef INC_LOGIC_MANAGER_H_
#define	INC_LOGIC_MANAGER_H_

#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include "SLB\SLB.hpp"

class Character {
	std::string name;

public:
	int life;

	Character() {}
	void setName(const char* aname) {
		name = aname;
	}
	const char* getName() {
		return name.c_str();
	}
};


class LogicManager
{
public:
	void bind(SLB::Manager& m);
	void run();
};

#endif
