
#include "initWindow.h"
#include "initAudio.h"
#include "initSprites.h"
#include "frameLoop.h"
#include "cleanup.h"

int main(int argc, char **argv) {
	initWindow();
	initAudio();
	initSprites();
	frameLoop();
	cleanup();
	return 0;
}
