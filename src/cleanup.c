#include <SDL2/SDL.h>
#include "initSprites.h"
#include "initWindow.h"

void cleanup(void) {
	cleanupSprites();
	SDL_GL_DeleteContext(GLcontext);_sdlec
	SDL_Quit();_sdlec
}
