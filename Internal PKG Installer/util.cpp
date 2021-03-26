#include "util.h"

#include <orbis/libkernel.h>

int system_notification(const char* text, const char* iconName) {
	OrbisSystemNotificationBuffer NotificationBuffer;
	
	NotificationBuffer.Type = OrbisSystemNotificationType::NotificationRequest;
	NotificationBuffer.unk3 = 0; 
	NotificationBuffer.UseIconImageUri = 1;
	NotificationBuffer.TargetId = -1;
	
	snprintf(NotificationBuffer.Uri, sizeof(NotificationBuffer.Uri), "cxml://psnotification/tex_%s", iconName);
	strncpy(NotificationBuffer.Message, text, sizeof(NotificationBuffer.Message));
	
	sceKernelSendNotificationRequest(0, (char*)&NotificationBuffer, 3120, 0);
	
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