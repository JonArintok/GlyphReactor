
#include <SDL2/SDL.h>
#include "initWindow.h"
#include "optionsAndErrors.h"

void cleanup(void) {
	SDL_GL_DeleteContext(GLcontext);_sdlec
	SDL_Quit();_sdlec
}
