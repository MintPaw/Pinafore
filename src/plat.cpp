#include <SDL2/SDL.h>
#include <GL/glew.h>

#include <stdio.h>
#include <time.h>
#include <cmath>

#undef GL_POLYGON_MODE
#define IMGUI_IMPL_OPENGL_LOADER_GLEW
#include "imgui.cpp"
#include "imgui_widgets.cpp"
#include "imgui_demo.cpp"
#include "imgui_draw.cpp"
#include "imgui_impl_sdl.cpp"
#include "imgui_impl_opengl3.cpp"

#ifndef GAME_NAME
# define GAME_NAME unnamed
#endif

#define STRINGIFYX(val) #val
#define STRINGIFY(val) STRINGIFYX(val)


#ifdef _WIN32
# define WIN32_LEAN_AND_MEAN
# include <Windows.h>
# include <timeapi.h>
# include <wchar.h>
# include <tchar.h>
#endif

#ifdef __linux__
# include <time.h>
#endif

#ifdef __EMSCRIPTEN__
# include <time.h>
# define GL_ES
# include <emscripten.h>
#else
# define GL_CORE
#endif

typedef int16_t int16;
typedef int32_t int32;
typedef uint16_t uint16;
typedef uint32_t uint32;

enum KeyState { KEY_RELEASED = 0, KEY_PRESSED, KEY_JUST_RELEASED, KEY_JUST_PRESSED };
enum PlatformKey { KEY_LEFT=301, KEY_RIGHT=302, KEY_UP=303, KEY_DOWN=304, KEY_SHIFT=305, KEY_BACKSPACE=306, KEY_CTRL=307, KEY_BACKTICK=308 };

struct NanoTime {
	unsigned int seconds;
	unsigned int nanos;
};

void initPlatform(int windowWidth, int windowHeight, const char *filePathPrefix="");
void platformUpdateLoop(void (*updateCallback)());
void platformUpdate();
bool fileExists(const char *filename);
void writeFile(const char *filename, void *data, int length);
void *readFile(const char *filename, int *outSize=NULL);
void swapBuffers();
void getNanoTime(NanoTime *time);

#define Assert(expr) platformAssert(expr, __FILE__, __LINE__)

#define KEY_LIMIT 500

void updateEvents();
void platformAssert(bool expr, const char *filename, int lineNum);
void platformSleep(int ms);

struct Platform {
	SDL_Window *sdlWindow;
	SDL_GLContext sdlContext;

	int windowWidth;
	int windowHeight;

	bool running;
	bool usingImgui;

	KeyState keys[KEY_LIMIT];
	char filePathPrefix[256];
	int mouseX;
	int mouseY;
	bool mouseDown;
	bool mouseJustDown;
	bool hoveringGui;

	int frameStartTime;
	int frameTime;
	float elapsed;
	float time;
	int frameCount;

	void (*updateCallback)();
};

Platform *platform = NULL;

#include "hotloader.cpp"

void initPlatform(int windowWidth, int windowHeight, const char *filePathPrefix) {
	setbuf(stdout, NULL);
	srand(time(NULL));
	printf("Platform Init\n");

	SetProcessDPIAware();

	platform = (Platform *)malloc(sizeof(Platform));
	memset(platform, 0, sizeof(Platform));
	platform->windowWidth = windowWidth;
	platform->windowHeight = windowHeight;
	strcpy(platform->filePathPrefix, filePathPrefix);

	Assert(!SDL_Init(SDL_INIT_VIDEO));

#ifdef GL_CORE
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
#else
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	// SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	// SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
#endif

	platform->sdlWindow = SDL_CreateWindow(STRINGIFY(GAME_NAME), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, platform->windowWidth, platform->windowHeight, SDL_WINDOW_OPENGL|SDL_WINDOW_ALLOW_HIGHDPI);
	Assert(platform->sdlWindow);

	platform->sdlContext = SDL_GL_CreateContext(platform->sdlWindow);
	Assert(platform->sdlContext);

	if (SDL_GL_SetSwapInterval(0)) {
		printf("Failed to set swap interval\n");
	}

	glewExperimental = GL_TRUE; 
	Assert(glewInit() == GLEW_OK);

	/// Imgui
	platform->usingImgui = true;
	if (platform->usingImgui) {
		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();
		io.IniFilename = NULL;
		ImGui_ImplSDL2_InitForOpenGL(platform->sdlWindow, platform->sdlContext);
#ifdef GL_ES
		ImGui_ImplOpenGL3_Init("#version 300 es");
#else
		ImGui_ImplOpenGL3_Init("#version 330");
#endif
		ImGui::StyleColorsDark();
	}

#ifdef _WIN32
	timeBeginPeriod(1);
#endif

	initHotloader("assets");
}

void platformUpdateLoop(void (*updateCallback)()) {
	platform->running = true;
	platform->updateCallback = updateCallback;

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(platformUpdate, 0, 1);
#else
	while (platform->running) {
		platformUpdate();
	}
#endif
}

void platformStartFrame() {
	platform->elapsed = 1/60.0;

	platform->frameStartTime = SDL_GetTicks();
	platform->time += platform->elapsed;
	platform->frameCount++;

	updateEvents();

	if (platform->usingImgui) {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(platform->sdlWindow);
		ImGui::NewFrame();
	}
}

void platformUpdate() {
	platform->updateCallback();

	updateHotloader();

	platform->frameTime = SDL_GetTicks() - platform->frameStartTime;
	int sleepMs = ceil(1.0/60.0*1000.0) - platform->frameTime;
	if (sleepMs > 0) platformSleep(sleepMs);
}

void swapBuffers() {
	if (platform->usingImgui) {
		// ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	SDL_GL_SwapWindow(platform->sdlWindow);
}

void updateEvents() {
	platform->mouseJustDown = false;

	for (int i = 0; i < KEY_LIMIT; i++) {
		if (platform->keys[i] == KEY_JUST_PRESSED) platform->keys[i] = KEY_PRESSED;
		else if (platform->keys[i] == KEY_JUST_RELEASED) platform->keys[i] = KEY_RELEASED;
	}

	SDL_Event e;
	while (SDL_PollEvent(&e) != 0) {
		if (platform->usingImgui) {
			ImGui_ImplSDL2_ProcessEvent(&e);
		}

		if (e.type == SDL_QUIT) {
			platform->running = false;
		} else if (e.type == SDL_KEYDOWN) {
			int key = e.key.keysym.sym;
			if (key == 27) platform->running = false;

			if (key >= 'a' && key <= 'z') key -= 'a'-'A';
			if (key == SDLK_LSHIFT) key = KEY_SHIFT;
			if (key == SDLK_RSHIFT) key = KEY_SHIFT;
			if (key == SDLK_BACKSPACE) key = KEY_BACKSPACE;
			if (key == SDLK_LCTRL) key = KEY_CTRL;
			if (key == SDLK_RCTRL) key = KEY_CTRL;
			if (key == SDLK_UP) key = KEY_UP;
			if (key == SDLK_DOWN) key = KEY_DOWN;
			if (key == SDLK_LEFT) key = KEY_LEFT;
			if (key == SDLK_RIGHT) key = KEY_RIGHT;
			if (key == SDLK_BACKQUOTE) key = KEY_BACKTICK;
			if (key > KEY_LIMIT) return;

			if (platform->keys[key] == KEY_RELEASED || platform->keys[key] == KEY_JUST_RELEASED) {
				platform->keys[key] = KEY_JUST_PRESSED;
			}	
		} else if (e.type == SDL_KEYUP) {
			int key = e.key.keysym.sym;
			if (key >= 'a' && key <= 'z') key -= 'a'-'A';
			if (key == SDLK_LSHIFT) key = KEY_SHIFT;
			if (key == SDLK_RSHIFT) key = KEY_SHIFT;
			if (key == SDLK_BACKSPACE) key = KEY_BACKSPACE;
			if (key == SDLK_LCTRL) key = KEY_CTRL;
			if (key == SDLK_RCTRL) key = KEY_CTRL;
			if (key == SDLK_UP) key = KEY_UP;
			if (key == SDLK_DOWN) key = KEY_DOWN;
			if (key == SDLK_LEFT) key = KEY_LEFT;
			if (key == SDLK_RIGHT) key = KEY_RIGHT;
			if (key == SDLK_BACKQUOTE) key = KEY_BACKTICK;
			if (key > KEY_LIMIT) return;

			if (platform->keys[key] == KEY_PRESSED || platform->keys[key] == KEY_JUST_PRESSED) {
				platform->keys[key] = KEY_JUST_RELEASED;
			}	
		}	else if (e.type == SDL_MOUSEMOTION) {
			SDL_GetMouseState(&platform->mouseX, &platform->mouseY);
		} else if (e.type == SDL_MOUSEBUTTONDOWN) {
			platform->mouseDown = true;
			platform->mouseJustDown = true;
		} else if (e.type == SDL_MOUSEBUTTONUP) {
			platform->mouseDown = false;
		} else if (e.type == SDL_MOUSEWHEEL) {
			// platformMouseWheel = e.wheel.y;
		}
	}

	ImGuiIO &io = ImGui::GetIO();
	platform->hoveringGui = io.WantCaptureMouse;
}

bool keyPressed(int key) {
	if (platform->keys[key] == KEY_JUST_PRESSED || platform->keys[key] == KEY_PRESSED) return true;
	return false;
}

bool keyJustPressed(int key) {
	if (platform->keys[key] == KEY_JUST_PRESSED) return true;
	return false;
}

bool keyJustReleased(int key) {
	if (platform->keys[key] == KEY_JUST_RELEASED) return true;
	return false;
}

bool fileExists(const char *filename) {
	char realName[256];
	if (platform) {
		strcpy(realName, platform->filePathPrefix);
		strcat(realName, filename);
	} else { // May be looking for assets file before initPlatform
		strcpy(realName, filename);
	}

	FILE *filePtr = fopen(realName, "rb");
	if (!filePtr) return false;

	fclose(filePtr);
	return true;
}

void writeFile(const char *filename, void *data, int length) {
	char realName[256];
	strcpy(realName, platform->filePathPrefix);
	strcat(realName, filename);

	FILE *filePtr = fopen(realName, "wb");
	if (!filePtr) {
		printf("Cannot find file %s\n", realName);
		Assert(0);
	}

	fwrite(data, length, 1, filePtr);
	fclose(filePtr);
}

void *readFile(const char *filename, int *outSize) {
	char realName[256];
	strcpy(realName, platform->filePathPrefix);
	strcat(realName, filename);

	FILE *filePtr = fopen(realName, "rb");
	if (!filePtr) {
		printf("Cannot find file %s\n", realName);
		Assert(0);
	}

	fseek(filePtr, 0, SEEK_END);
	long fileSize = ftell(filePtr);
	fseek(filePtr, 0, SEEK_SET);

	char *str = (char *)malloc(fileSize + 1);
	fread(str, fileSize, 1, filePtr);
	fclose(filePtr);

	str[fileSize] = 0;

	if (outSize) *outSize = fileSize;

	return str;
}

void platformAssert(bool expr, const char *filename, int lineNum) {
	if (!expr) {
		printf("Assert failed at %s line %d\n", filename, lineNum);
		*(char *)0 = 0;
	}
}

void platformSleep(int ms) {
#ifdef _WIN32
	Sleep(ms);
#elif __linux__
	timespec ts;
	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000;
	nanosleep(&ts, NULL);
#endif
}

void getNanoTime(NanoTime *time) {
#ifdef _WIN32
	LARGE_INTEGER winTime;
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq); 
	QueryPerformanceCounter(&winTime);
	unsigned int nanos = winTime.QuadPart * 1000000000 / freq.QuadPart;
	time->seconds = nanos / 1000000000;
	time->nanos = nanos % 1000000000;
#endif

#ifdef __linux__
	timespec ts;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
	time->seconds = ts.tv_sec;
	time->nanos = ts.tv_nsec;
#endif

#ifdef __EMSCRIPTEN__
	timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	time->seconds = ts.tv_sec;
	time->nanos = ts.tv_nsec;
#endif
}

float getMsPassed(NanoTime *startTime, NanoTime *endTime) {
	NanoTime time;

	if (endTime->nanos < startTime->nanos) {
		time.seconds = endTime->seconds - startTime->seconds - 1;
		time.nanos = 1000000000 + endTime->nanos - startTime->nanos;
	} else {
		time.seconds = endTime->seconds - startTime->seconds;
		time.nanos = endTime->nanos - startTime->nanos;
	}

	return time.seconds * 1000.0 + time.nanos * 0.000001;
}
