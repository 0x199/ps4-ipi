#pragma once

#include "common.h"
#include <orbis/libkernel.h>
#include <fcntl.h>
#include <libjbc.h>

struct RunAsRootArgs {
	int (*funcPtr)();
	int returnValue;
};

int sudo_func(int (*funcPtr)());
int system_notification(const char* text, const char* iconName = "icon_system");
int ends_with(const char* haystack, const char* needle);
