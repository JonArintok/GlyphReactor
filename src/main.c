
#include "initWindow.h"
#include "initSprites.h"
#include "audio.h"
#include "frameLoop.h"
#include "cleanup.h"

int main(int argc, char **argv) {
	initWindow();
	initAudio();
	initSprites();
	frameLoop();
	return 0;
}
