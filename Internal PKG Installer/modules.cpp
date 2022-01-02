#include "modules.h"

#include <orbis/libkernel.h>
#include <orbis/Sysmodule.h>

static bool s_modules_loaded = false;

bool(*jailbreak)();

int (*sceAppInstUtilGetTitleIdFromPkg)(const char* pkgPath, char* titleId, int* isApp);

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

int load_extra_modules(void) {
	int app_util_handle = sceKernelLoadStartModule("/system/common/lib/libSceAppInstUtil.sprx", 0, NULL, 0, NULL, NULL);
	if (app_util_handle == 0) {
		printf("sceKernelLoadStartModule() failed to load module %s\n", "libSceAppInstUtil.sprx");
		return -1;
	}

	sceKernelDlsym(app_util_handle, "sceAppInstUtilGetTitleIdFromPkg", (void**)&sceAppInstUtilGetTitleIdFromPkg);
	if (sceAppInstUtilGetTitleIdFromPkg == NULL) {
		printf("Failed to resolve symbol: %s\n", "sceAppInstUtilGetTitleIdFromPkg");
		return -1;
	}
	return 0;
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
