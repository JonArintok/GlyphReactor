
#include "initWindow.h"
#include "initSprites.h"
#include "initBounceEnv.h"
#include "initOpenGl.h"
#include "frameLoop.h"

int main(int argc, char **argv) {
	initWindow();
	initSprites();
	initBounceEnv();
	initOpenGl();
	frameLoop();
	return 0;
}
