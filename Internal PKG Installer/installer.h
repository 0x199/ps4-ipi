#pragma once

#include "common.h"
#include "util.h"
#include "modules.h"

int bgft_init(void);
void bgft_fini(void);

int install_pkg(const char* filename);