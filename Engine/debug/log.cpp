#include "mcv_platform.h"
#include "log.h"
#include <ctime>

char* CLog::file_log = "log.txt";

void CLog::reset()
{
	std::ofstream ofs(file_log, std::ofstream::out);
	ofs.close();
}

void CLog::append(const char* txt)
{
	std::ofstream ofs(file_log, std::ofstream::app);
	ofs << currentDateTime() + ": " + txt + "\n";
	ofs.close();
}

void CLog::appendFormat(const char* format, ...) {
	va_list argptr;
	va_start(argptr, format);
	char dest[1024 * 16];
	_vsnprintf(dest, sizeof(dest), format, argptr);
	va_end(argptr);
	append(dest);
}

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const std::string CLog::currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

	return buf;
}