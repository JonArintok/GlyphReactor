#include <SDL2/SDL.h>
#include "initWindow.h"
#include "optionsAndErrors.h"
#include "initSprites.h"

void cleanup(void) {
	free(spiroSprites); // malloc in "initSprites.c"
	free(visSpiros); // malloc in "initSprites.c"
	free(glyphSpiros); // malloc in "initSprites.c"
	SDL_GL_DeleteContext(GLcontext);_sdlec
	SDL_Quit();_sdlec
}
