#include "installer.h"

#include <libgen.h>

#include <orbis/libkernel.h>
#include <orbis/Bgft.h>

#define BGFT_HEAP_SIZE (1 * 1024 * 1024)

static OrbisBgftInitParams s_bgft_init_params;

static bool s_bgft_initialized = false;

int bgft_init(void) {
	int ret;

	if (s_bgft_initialized) {
		goto done;
	}

	memset(&s_bgft_init_params, 0, sizeof(s_bgft_init_params));
	{
		s_bgft_init_params.heapSize = BGFT_HEAP_SIZE;
		s_bgft_init_params.heap = (uint8_t*)malloc(s_bgft_init_params.heapSize);
		if (!s_bgft_init_params.heap) {
			printf("No memory for BGFT heap.\n");
			goto err;
		}
		memset(s_bgft_init_params.heap, 0, s_bgft_init_params.heapSize);
	}

	ret = sceBgftServiceIntInit(&s_bgft_init_params);
	if (ret) {
		printf("sceBgftServiceIntInit failed: 0x%08X\n", ret);
		goto err_bgft_heap_free;
	}

	s_bgft_initialized = true;

done:
	return 0;

err_bgft_heap_free:
	if (s_bgft_init_params.heap) {
		free(s_bgft_init_params.heap);
		s_bgft_init_params.heap = NULL;
	}

	memset(&s_bgft_init_params, 0, sizeof(s_bgft_init_params));

err:
	s_bgft_initialized = false;

	return -1;
}

void bgft_fini(void) {
	int ret;

	if (!s_bgft_initialized) {
		return;
	}

	ret = sceBgftServiceIntTerm();
	if (ret) {
		printf("sceBgftServiceIntTerm failed: 0x%08X\n", ret);
	}

	if (s_bgft_init_params.heap) {
		free(s_bgft_init_params.heap);
		s_bgft_init_params.heap = NULL;
	}

	memset(&s_bgft_init_params, 0, sizeof(s_bgft_init_params));

	s_bgft_initialized = false;
}

int install_pkg(const char* filename) {
	int ret;
	
	char filepath[255];
	snprintf(filepath, sizeof(filepath), "/user/data/%s", filename);
	
	char titleId[18];
	memset(titleId, 0 ,sizeof(titleId));
	int is_app = -1;
	ret = sceAppInstUtilGetTitleIdFromPkg(filepath, titleId, &is_app);
	if (ret) {
		printf("Cannot get title id %s: BGFT Error 0x%08X\n", filepath, ret);
		return -1;
	}


	printf("New install task: %s\n", filepath);
	
	OrbisBgftDownloadParamEx download_params;
	memset(&download_params, 0, sizeof(download_params));
	{
		download_params.params.entitlementType = 5;
		download_params.params.id = "";
		download_params.params.contentUrl = filepath;
		download_params.params.contentName = titleId;
		download_params.params.iconPath = "";
		download_params.params.playgoScenarioId = "0";
		download_params.params.option = ORBIS_BGFT_TASK_OPT_DISABLE_CDN_QUERY_PARAM;
		download_params.slot = 0;
	}
	
	int task_id = -1;
	ret = sceBgftServiceIntDownloadRegisterTaskByStorageEx(&download_params, &task_id);
	if (ret) {
		printf("Cannot install %s: BGFT Error 0x%08X\n", filepath, ret);
		goto err;
	}
	EPRINTF("Task ID: 0x%08X\n", task_id);

	ret = sceBgftServiceDownloadStartTask(task_id);
	if (ret) {
		printf("sceBgftServiceDownloadStartTask failed: 0x%08X\n", ret);
		goto err;
	}
	
	return 0;
	
err:
	return -1;
}
