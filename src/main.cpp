#include "memoryTools.cpp"

#define GAME_NAME Pinafore
#include "plat.cpp"

#define TINYTILED_IMPLEMENTATION
#include "tinytiled.h"

#include "mathTools.cpp"
#include "renderer.cpp"
#include "font.cpp"
#include "audio.cpp"
#include "save.cpp"
#include "catalog.cpp"
#include "emitter.cpp"
#include "game.cpp"

#ifdef _MSC_VER
INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
#else
int main(int argc, char **argv)
#endif
{
	char assetPrefix[256];
	strcpy(assetPrefix, "");
	if (!fileExists("assets/sprites.spr")) {
		printf("Nested assets not found!\n");
		if (fileExists("../assets/sprites.spr")) {
			printf("Found source assets\n");
			strcpy(assetPrefix, "../");
		}
	}

	initPlatform(1280, 720, assetPrefix);

	initAudio();
	initRenderer(1280, 720);
	platformUpdateLoop(updateGame);

	return 0;
}
