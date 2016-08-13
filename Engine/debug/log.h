#ifndef INC_DEBUG_LOG_H_
#define INC_DEBUG_LOG_H_

class CLog {
	static char* file_log;
	static const std::string currentDateTime();
public:
	static void reset();
	static void append(const char* txt);
};

#endif