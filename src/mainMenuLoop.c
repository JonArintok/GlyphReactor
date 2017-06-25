
#include <stdio.h>
#include <SDL2/SDL.h>

#include "initOpenGl.h"
#include "initSprites.h"
#include "glyphReactorLoop.h"

int mainMenuLoop(char charEntered, int curFrame) {
	switch (charEntered) {
		case SDLK_ESCAPE: return quitGame;
		case SDLK_RETURN:
			initWordQueueSprites("courses/home row.txt");
			initGlyphReactorLoop();
			return glyphReactor;
		case SDLK_TAB:
			glUniform2f(unif_translate, 0, 0);
			return spiroViewer;
	}
	glUniform2f(unif_translate, txtOriginX_, txtOriginY_);
	glDrawArrays(GL_POINTS, visCharVertBeg_, visCharCount_);
	return mainMenu;
}
