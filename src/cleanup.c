#include <SDL2/SDL.h>
#include "initSprites.h"
#include "initWindow.h"
#include "audio.h"

void cleanup(void) {
	cleanupSprites();
	closeAudio();
	SDL_GL_DeleteContext(GLcontext);_sdlec
	SDL_Quit();_sdlec
}
