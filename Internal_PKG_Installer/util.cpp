#include "util.h"



void run_as_root(void * args) {
	struct RunAsRootArgs * runAsRootArgs = (struct RunAsRootArgs*)args;
	runAsRootArgs->returnValue = runAsRootArgs->funcPtr();
}

int sudo_func(int (*funcPtr)()) {
	struct RunAsRootArgs args;
	args.funcPtr = funcPtr;
	jbc_run_as_root(run_as_root, &args, CWD_ROOT);
	return args.returnValue;
}

int system_notification(const char* text, const char* iconName) {
	OrbisNotificationRequest NotificationBuffer;
	
	NotificationBuffer.type = OrbisNotificationRequestType::NotificationRequest;
	NotificationBuffer.unk3 = 0; 
	NotificationBuffer.useIconImageUri = 1;
	NotificationBuffer.targetId = -1;
	
	snprintf(NotificationBuffer.iconUri, sizeof(NotificationBuffer.iconUri), "cxml://psnotification/tex_%s", "icon_system");
	strncpy(NotificationBuffer.message, text, sizeof(NotificationBuffer.message));
	sceKernelSendNotificationRequest(0, &NotificationBuffer, 3120, 0);

	return 0;
}

int ends_with(const char* haystack, const char* needle) {
	ptrdiff_t diff;
	int i;

	assert(haystack != NULL);
	assert(needle != NULL);

	diff = strlen(haystack) - strlen(needle);
	if (diff < 0) {
		return -1;
	}

	for (i = 0; needle[i] != '\0'; ++i) {
		if (needle[i] != haystack[i + diff]) {
			return -2;
		}
	}

	return 0;
}
