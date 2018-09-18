#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <timeapi.h>
#include <stdio.h>
#include <time.h>

#ifndef GAME_NAME
# define GAME_NAME unnamed
#endif

#define STRINGIFYX(val) #val
#define STRINGIFY(val) STRINGIFYX(val)

#ifdef __linux__
# include <time.h>
#endif

enum KeyState { KEY_RELEASED = 0, KEY_PRESSED, KEY_JUST_RELEASED, KEY_JUST_PRESSED };
enum PlatformKey { KEY_LEFT=301, KEY_RIGHT=302, KEY_UP=303, KEY_DOWN=304, KEY_SHIFT=305, KEY_BACKSPACE=306, KEY_CTRL=307, KEY_BACKTICK=308 };

struct NanoTime {
	unsigned int seconds;
	unsigned int nanos;

#ifdef _WIN32
	LARGE_INTEGER winFreq;
	LARGE_INTEGER time;
#endif
};

void initPlatform(int windowWidth, int windowHeight);
void platformUpdateLoop(void (*updateCallbcak)());
long readFile(const char *filename, void **storage);
void swapBuffers();
void getNanoTime(NanoTime *time);

#define Assert(expr) platformAssert(expr, __FILE__, __LINE__)
#define GL_CORE
// #define GL_ES

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

	KeyState keys[KEY_LIMIT];
	int mouseX;
	int mouseY;
	bool mouseDown;
	bool mouseJustDown;

	int frameTime;
	float elapsed;
	float time;
	int frameCount;
};

Platform *platform;

void initPlatform(int windowWidth, int windowHeight) {
	setbuf(stdout, NULL);
	srand(time(NULL));
	printf("Platform Init\n");

	platform = (Platform *)malloc(sizeof(Platform));
	memset(platform, 0, sizeof(Platform));
	platform->windowWidth = windowWidth;
	platform->windowHeight = windowHeight;

#ifdef _WIN32
	timeBeginPeriod(1);
#endif

	Assert(!SDL_Init(SDL_INIT_VIDEO));

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	platform->sdlWindow = SDL_CreateWindow(STRINGIFY(GAME_NAME), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, platform->windowWidth, platform->windowHeight, SDL_WINDOW_OPENGL);
	Assert(platform->sdlWindow);

	platform->sdlContext = SDL_GL_CreateContext(platform->sdlWindow);
	Assert(platform->sdlContext);

	Assert(!SDL_GL_SetSwapInterval(0));

	glewExperimental = GL_TRUE; 
	Assert(glewInit() == GLEW_OK);
}

void platformUpdateLoop(void (*updateCallbcak)()) {
	platform->running = true;
	while (platform->running) {
		platform->elapsed = 1/60.0;

		int startTime = SDL_GetTicks();
		platform->time += platform->elapsed;
		platform->frameCount++;

		updateEvents();
		updateCallbcak();

		platform->frameTime = SDL_GetTicks() - startTime;
		int sleepMs = ceilf(1.0/60.0*1000.0) - platform->frameTime;
		if (sleepMs > 0) platformSleep(sleepMs);
	}
}

void swapBuffers() {
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

long readFile(const char *filename, void **storage) {
	FILE *filePtr = fopen(filename, "rb");
	if (!filePtr) {
		printf("Cannot find file %s\n", filename);
		Assert(0);
	}

	fseek(filePtr, 0, SEEK_END);
	long fileSize = ftell(filePtr);
	fseek(filePtr, 0, SEEK_SET);  //same as rewind(f);

	char *str = (char *)malloc(fileSize + 1);
	fread(str, fileSize, 1, filePtr);
	fclose(filePtr);

	str[fileSize] = 0;

	*storage = str;

	return fileSize;
}

void platformAssert(bool expr, const char *filename, int lineNum) {
	if (!expr) {
		printf("Assert failed at %s line %d\n", filename, lineNum);
		exit(1);
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

	// time->time = StartingTime;
	// time->winFreq = Frequency;
#elif __linux__
	timespec ts;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
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
