#ifndef GLOBAL_H
#define GLOBAL_H

#include "config/config.h"
#include "time.h"
#include "input/input.h"

typedef struct global {
	Config_State config;
	Time_State time;
	Input_State input;
} Global;

extern Global global;

#endif // !GLOBAL_H
