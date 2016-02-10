#ifndef _SBB_H
#define _SBB_H

class sbb
{
	std::map<std::string, int> data;

public:

	void init();
	int read(std::string key);
	void write(std::string key, int value);

};

#endif