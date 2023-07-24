#pragma once

#include "../input/input.h"
#include "../types.h"

typedef struct config {
	u8 keybinds[5];
} Config_State;

void config_intit(void);
void config_key_bind(Input_Key key, const char* key_name);