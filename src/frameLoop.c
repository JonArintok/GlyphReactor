
#include <SDL2/SDL.h>

#include "initWindow.h"
#include "initOpenGl.h"
#include "initSprites.h"
#include "mainMenuLoop.h"
#include "glyphReactorLoop.h"
#include "spiroViewerLoop.h"
#include "optionsAndErrors.h"
#include "timestamp.h"
#include "../img/texAtlas.h"

bool handleEvents(char *charEntered) {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT: return false; break;
			case SDL_WINDOWEVENT:
				switch (event.window.event) {
					case SDL_WINDOWEVENT_RESIZED:
						videoW = event.window.data1;
						videoH = event.window.data2;
						glViewport(0, 0, videoW, videoH);
						glUniform2f(unif_scale, scaleX_, scaleY_);
					break;
				}
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
					case SDLK_BACKSPACE: *charEntered = bkspChar; break;
					case SDLK_ESCAPE:    *charEntered = SDLK_ESCAPE; break;
					case SDLK_RETURN:    *charEntered = SDLK_RETURN; break;
					case SDLK_TAB:       *charEntered = SDLK_TAB; break;
				}
				break;
			case SDL_TEXTINPUT: {
				*charEntered = event.text.text[0];
				break;
			}
			case SDL_KEYUP: break;
		}
	}
	return true;
}

int curFrame = 0;
timestamp ts_oldFrameStart={0,0},ts_newFrameStart={0,0},ts_frameDelta={0,0};
#ifdef LOG_TIMING_TO
timestamp ts_compTime = {0,0}, ts_now = {0,0};
#endif

void frameLoop(void) {
	clock_gettime(CLOCK_MONOTONIC, &ts_newFrameStart);
	for (bool running = true; running; curFrame++) {
		ts_oldFrameStart = ts_newFrameStart;
		clock_gettime(CLOCK_MONOTONIC, &ts_newFrameStart);
		getTimeDelta(&ts_oldFrameStart, &ts_newFrameStart, &ts_frameDelta);
		#ifdef LOG_TIMING_TO
		fprintf(LOG_TIMING_TO,
			"ts_frameDelta: %1ld s, %9ld ns\n",
			ts_frameDelta.tv_sec, ts_frameDelta.tv_nsec
		);
		#endif
		char charEntered = '\0';
		running = handleEvents(&charEntered);
		if (!running) break;
		switch (whereAreWe) {
			case mainMenu:
				whereAreWe = mainMenuLoop(charEntered, curFrame);
				break;
			case glyphReactor:
				whereAreWe = glyphReactorLoop(charEntered, curFrame);
				break;
			case spiroViewer:
				whereAreWe = spiroViewerLoop(charEntered, curFrame);
				break;
			default: running = false;
		}
		#ifdef LOG_TIMING_TO
		clock_gettime(CLOCK_MONOTONIC, &ts_now);
		getTimeDelta(&ts_newFrameStart, &ts_now, &ts_compTime);
		fprintf(LOG_TIMING_TO,
			"ts_compTime: %3ld s, %9ld ns\n",
			ts_compTime.tv_sec, ts_compTime.tv_nsec
		);
		#endif
		SDL_GL_SwapWindow(window);_sdlec
		glClear(GL_COLOR_BUFFER_BIT);
	}
	free(chars);             // malloc in "initSprites.c"
	free(fileNames);         // malloc in "initSprites.c"
	free(gunSprites);        // malloc in "initSprites.c"
	free(menuCursorSprites); // malloc in "initSprites.c"
	free(charSprites);       // malloc in "initSprites.c"
	free(beamSprites);       // malloc in "initSprites.c"
	SDL_GL_DeleteContext(GLcontext);_sdlec // init int "initWindow.c"
	SDL_Quit();_sdlec                      // init int "initWindow.c"
}
