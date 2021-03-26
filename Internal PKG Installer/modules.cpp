#include "modules.h"

#include <orbis/libkernel.h>
#include <orbis/Sysmodule.h>

static bool s_modules_loaded = false;

bool(*jailbreak)();

int load_modules(void) {
	int ret, jbc_handle;

	ret = sceSysmoduleLoadModuleInternal(ORBIS_SYSMODULE_INTERNAL_BGFT);
	if (ret) {
		printf("sceSysmoduleLoadModuleInternal(%s) failed: 0x%08X\n", STRINGIFY_DEEP(ORBIS_SYSMODULE_INTERNAL_BGFT), ret);
		goto err;
	}
	
	// https://github.com/sleirsgoevy/ps4-libjbc
	jbc_handle = sceKernelLoadStartModule("/app0/sce_module/libjbc.sprx", 0, NULL, 0, NULL, NULL);
	if (jbc_handle == 0) {
		printf("sceKernelLoadStartModule() failed to load module %s\n", "libjbc.sprx");
		goto err;
	}
	
	sceKernelDlsym(jbc_handle, "Jailbreak", (void**)&jailbreak);
	if(jailbreak == nullptr) {
		printf("Failed to resolve symbol: %s\n", "Jailbreak");
		goto err;
	}
	
	s_modules_loaded = true;
	
done:
	return 0;
	
err:
	return -1;
}

void unload_modules(void) {
	int ret;
	
	if (!s_modules_loaded) {
		return;
	}
	
	ret = sceSysmoduleUnloadModuleInternal(ORBIS_SYSMODULE_INTERNAL_BGFT);
	if (ret) {
		EPRINTF("sceSysmoduleUnloadModule(%s) failed: 0x%08X\n", STRINGIFY_DEEP(ORBIS_SYSMODULE_INTERNAL_BGFT), ret);
	}
	
	s_modules_loaded = false;
}