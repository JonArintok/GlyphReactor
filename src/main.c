
#include "initWindow.h"
#include "initSprites.h"
#include "frameLoop.h"

int main(int argc, char **argv) {
	initWindow();
	initSprites();
	frameLoop();
	return 0;
}
