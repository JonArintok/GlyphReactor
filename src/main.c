
#include "initWindow.h"
#include "initSprites.h"
#include "frameLoop.h"
#include "cleanup.h"

int main(int argc, char **argv) {
	initWindow();
	initSprites();
	frameLoop();
	cleanup();
	return 0;
}
