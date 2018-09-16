#include "memoryTools.cpp"

#define GAME_NAME pinafore
#include "plat.cpp"

#define TINYTILED_IMPLEMENTATION
#include "tinytiled.h"

#include "mathTools.cpp"
#include "renderer.cpp"
#include "font.cpp"
#include "game.cpp"

#ifdef _MSC_VER
INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
#else
int main(int argc, char **argv)
#endif
{
	initPlatform(1280, 720);
	initRenderer(1280, 720);
	platformUpdateLoop(updateGame);

	return 0;
}
