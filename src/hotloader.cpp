#define WATCHED_FILES_LIMIT 256
#define WINDOWS_TICK 10000000
#define SEC_TO_UNIX_EPOCH 11644473600LL

struct WatchedFile {
	const char *path;
	unsigned int lastWriteTime;
};

struct Hotloader {
	WatchedFile watchedFiles[WATCHED_FILES_LIMIT];
	int watchedFilesNum;

#ifdef _WIN32
	HANDLE changeHandle;
#endif

	void (*fileUpdateCallback)(const char *);
};

Hotloader *hotloader = NULL;
void initHotloader();
void watchFile(const char *path);
void updateHotloader();
void scanFiles();
void reloadAllWatched();

void initHotloader(const char *watchDir) {
	hotloader = (Hotloader *)malloc(sizeof(Hotloader));
	memset(hotloader, 0, sizeof(Hotloader));
#ifdef _WIN32

	HMODULE hModule = GetModuleHandleW(NULL);
	WCHAR absPathW[256];
	GetModuleFileNameW(hModule, absPathW, 256);

	char absPath[256];
	wcstombs(absPath, absPathW, 256);

	char *lastSlash = strrchr(absPath, '\\');
	*lastSlash = '\0';
	strcat(absPath, "\\");

	strcat(absPath, platform->filePathPrefix);
	strcat(absPath, watchDir);

	int realPathLen = strlen(absPath);
	for (int i = 0; i < realPathLen; i++) {
		if (absPath[i] == '/') absPath[i] = '\\';
	}

	printf("Hotloader path set: %s\n", absPath);
	hotloader->changeHandle = FindFirstChangeNotification(
		_T(absPath),
		TRUE,
		FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_DIR_NAME|FILE_NOTIFY_CHANGE_SIZE|FILE_NOTIFY_CHANGE_LAST_WRITE
	);

	if (hotloader->changeHandle == INVALID_HANDLE_VALUE) printf("\n ERROR: FindFirstChangeNotification function failed.\n");
	if (hotloader->changeHandle == NULL) printf("\n ERROR: Unexpected NULL from FindFirstChangeNotification.\n");
#endif
}

void watchFile(const char *path) {
	WatchedFile *watch = &hotloader->watchedFiles[hotloader->watchedFilesNum++];
	watch->path = stringClone(path);
}

void updateHotloader() {
#ifdef _WIN32
	DWORD fileWatchWaitStatus = WaitForSingleObject(hotloader->changeHandle, 0);
	if (fileWatchWaitStatus == WAIT_OBJECT_0) {
		scanFiles();
		// printf("A file was changed\n");
		if (FindNextChangeNotification(hotloader->changeHandle) == FALSE) printf("\n ERROR: FindNextChangeNotification function failed.\n");
	} else if (fileWatchWaitStatus == WAIT_TIMEOUT) {
		// No files changed
	}
#endif
}

void scanFiles() {
#ifdef _WIN32
	for (int i = 0; i < hotloader->watchedFilesNum; i++) {
		WatchedFile *watch = &hotloader->watchedFiles[i];

		char realPath[256];
		strcpy(realPath, platform->filePathPrefix);
		strcat(realPath, watch->path);

		int realPathLen = strlen(realPath);
		for (int i = 0; i < realPathLen; i++) {
			if (realPath[i] == '/') realPath[i] = '\\';
		}

		HANDLE fileHandle = CreateFile(realPath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		if (fileHandle == INVALID_HANDLE_VALUE) {
			printf("Failed to scan file %s!\n", watch->path);
			continue;
		}

		FILETIME writeTime;
		BOOL succ = GetFileTime(fileHandle, NULL, NULL, &writeTime);
		ULARGE_INTEGER ull;
		ull.LowPart = writeTime.dwLowDateTime;
		ull.HighPart = writeTime.dwHighDateTime;

		unsigned int lastWriteTime = ull.QuadPart / 10000000ULL - 11644473600ULL;

		if (watch->lastWriteTime != lastWriteTime) {
			watch->lastWriteTime = lastWriteTime;
			if (hotloader->fileUpdateCallback) hotloader->fileUpdateCallback(watch->path);
			// printf("File %s would be reloaded\n", watch->path);
		}

		CloseHandle(fileHandle);
	}
#endif
}

void reloadAllWatched() {
	for (int i = 0; i < hotloader->watchedFilesNum; i++) {
		WatchedFile *watch = &hotloader->watchedFiles[i];
		if (hotloader->fileUpdateCallback) hotloader->fileUpdateCallback(watch->path);
	}
}
