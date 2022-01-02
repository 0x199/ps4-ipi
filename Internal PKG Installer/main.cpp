#include <stdio.h>
#include <dirent.h>
#include <signal.h>

#include "common.h"
#include "modules.h"
#include "util.h"

#include <orbis/libkernel.h>
#include <orbis/SystemService.h>

static int check_directory(void);

static void cleanup(void);

int main() {
	setvbuf(stdout, NULL, _IONBF, 0);
	atexit(&cleanup);
	
	// Load needed modules
	if (load_modules()) {
		EPRINTF("Unable to load modules.\n");
		goto err;
	}
	
	// Escape from sandbox
	jailbreak();
	
	if (load_extra_modules()) {
		EPRINTF("Unable to load extra modules.\n");
		goto err;
	}

	// Initiate background file transfer
	if (bgft_init()) {
		EPRINTF("BGFT initialization failed.\n");
		goto err;
	}
	
	// Check for packages
	check_directory();
	
	// Clean exit the app
	sceSystemServiceLoadExec((char*)"exit", 0);
	
	return 0;
	
err:
	sceSystemServiceLoadExec((char*)"exit", 0);
	return 0;
}

static int check_directory() {
	DIR *dir = opendir("/data");
	dirent *entry;
	int found = 0;
	
	if(!dir) {
		return 1;
	}
	
	// Loop over all entries in directory
	while((entry = readdir(dir)) != NULL) {
		
		// If it's a directory OR a file not ending in .pkg, skip
		if(entry->d_type == DT_DIR || ends_with(entry->d_name, ".pkg")) {
			continue;
		}
		
		// Attempt to install PKG file
		if(install_pkg(entry->d_name)) {
			char errorMsg[100];
			snprintf(errorMsg, sizeof(errorMsg), "Cannot install:\n%s", entry->d_name);
			
			system_notification(errorMsg, "default_icon_download");
		}
		
		found++;
	}
	
	// No PKG files found
	if(found == 0) {
		system_notification("No packages found in /data");
	}

	closedir(dir);
	
	return 0;
}

static void cleanup(void) {
	unload_modules();
}