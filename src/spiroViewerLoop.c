
#include <stdio.h>
#include <SDL2/SDL.h>

#include "initSprites.h"
#include "drawSpiro.h"


int spiroViewerLoop(int charEntered, int curFrame) {
	switch (charEntered) {
		case SDLK_ESCAPE: return mainMenu;
		case '\0': break;
		default: triggerSpiro(charEntered); break;
	}
	drawSpiros();
	return spiroViewer;
}
