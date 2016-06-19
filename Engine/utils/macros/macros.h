#ifndef INC_MACROS_H_
#define INC_MACROS_H_

#define clamp(xval, xmin, xmax) (xval < xmin ? xmin : (xval > xmax ? xmax : xval))
#define clamp_me(xval, xmin, xmax) xval = clamp(xval, xmin, xmax)
#define clampAbs(xval, xmaxAbs) clamp(xval,-xmaxAbs, xmaxAbs)

#define sameSign(x,y) std::signbit(x) == std::signbit(y)
#include "timers.h"

#endif
