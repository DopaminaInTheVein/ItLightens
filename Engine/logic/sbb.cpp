#include "mcv_platform.h"
#include "sbb.h"

void sbb::init() {
	data["attacking_slots"] = 1;
}

int sbb::read(std::string key) {
	// try to find a key with the suitable name
	if (data.find(key) == data.end())
	{
		// if the key does not exist, we abort
		exit(-1);
	}
	return data[key];
}


void sbb::write(std::string key, int value) {
	// try to find a key with the suitable name
	if (data.find(key) == data.end())
	{
		// if the key does not exist, we abort
		exit(-1);
	}
	 data[key] = value;
}