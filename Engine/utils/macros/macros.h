#ifndef INC_MACROS_H_
#define INC_MACROS_H_

#define clamp(xval, xmin, xmax) (xval < xmin ? xmin : (xval > xmax ? xmax : xval))
#define clampAbs(xval, xmaxAbs) clamp(xval,-xmaxAbs, xmaxAbs)

#include "timers.h"

#endif
