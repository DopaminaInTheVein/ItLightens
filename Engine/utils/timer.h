#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#include "mcv_platform.h"

class CTimer {
  uint64_t start;

public:
  CTimer() : start(timeStamp()) {
  }

  uint64_t timeStamp() {
    LARGE_INTEGER num_ticks;
    ::QueryPerformanceCounter(&num_ticks);
    return num_ticks.QuadPart;
  }

  float elapsed() {
    uint64_t now = timeStamp();
    uint64_t delta = now - start;
    LARGE_INTEGER freq;
    ::QueryPerformanceFrequency(&freq);
    return (float)(double(delta) / double(freq.QuadPart));
  }

  float deltaAndReset() {
    uint64_t now = timeStamp();
    uint64_t delta = now - start;
    LARGE_INTEGER freq;
    ::QueryPerformanceFrequency(&freq);
    start = now;
    return (float)(double(delta) / double(freq.QuadPart));
  }
};


#endif

