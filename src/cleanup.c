#include <SDL2/SDL.h>
#include "initAudio.h"
#include "initSprites.h"
#include "initWindow.h"

void cleanup(void) {
	cleanupSprites();
	cleanupAudio();
	SDL_GL_DeleteContext(GLcontext);_sdlec
	SDL_Quit();_sdlec
}
