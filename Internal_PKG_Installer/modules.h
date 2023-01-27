#pragma once

#include "common.h"
#include "installer.h"
int load_modules(void);
int load_extra_modules(void);
void unload_modules(void);

extern int (*sceAppInstUtilGetTitleIdFromPkg)(const char* pkgPath, char* titleId, int* isApp);
