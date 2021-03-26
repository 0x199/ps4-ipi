#pragma once

#include "common.h"
#include "installer.h"

int load_modules(void);
void unload_modules(void);

extern bool(*jailbreak)();