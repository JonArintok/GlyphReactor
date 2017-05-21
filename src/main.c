
#include "initWindow.h"
#include "initSprites.h"
#include "initBounceEnv.h"
#include "initOpenGl.h"
#include "frameLoop.h"
#include "cleanup.h"

int main(int argc, char **argv) {
	initWindow();
	initSprites();
	initBounceEnv();
	initOpenGl();
	frameLoop();
	cleanup();
	return 0;
}
