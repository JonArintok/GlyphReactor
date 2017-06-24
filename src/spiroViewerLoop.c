
#include <stdio.h>
#include <SDL2/SDL.h>

#include "initSprites.h"
#include "drawSpiro.h"


int spiroViewerLoop(char charEntered, int curFrame) {
	switch (charEntered) {
		case SDLK_ESCAPE: return mainMenu;
		default: triggerSpiro(charEntered); break;
	}
	drawSpiros();
	return spiroViewer;
}
