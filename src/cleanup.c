#include <SDL2/SDL.h>
#include "initSprites.h"
#include "initWindow.h"
#include "voice.h"

void cleanup(void) {
	cleanupSprites();
	closeVoices();
	SDL_GL_DeleteContext(GLcontext);_sdlec
	SDL_Quit();_sdlec
}
