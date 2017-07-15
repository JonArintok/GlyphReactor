
#include "initWindow.h"
#include "initSprites.h"
#include "voice.h"
#include "frameLoop.h"
#include "cleanup.h"

int main(int argc, char **argv) {
	initWindow();
	initVoices(voiceCount);
	initSprites();
	frameLoop();
	return 0;
}
