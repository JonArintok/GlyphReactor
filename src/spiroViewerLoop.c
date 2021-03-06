
#include <stdio.h>
#include <SDL2/SDL.h>

#include "initSprites.h"
#include "drawSpiro.h"


int spiroViewerLoop(int charEntered, int curFrame) {
	switch (charEntered) {
		case SDLK_TAB: // fall
		case SDLK_ESCAPE: return screen_mainMenu;
		case SDLK_RETURN: break;
		case '\0': break;
		default: triggerSpiro(charEntered); break;
	}
	drawSpiros();
	return screen_spiroViewer;
}
