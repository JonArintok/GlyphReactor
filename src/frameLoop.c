
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

bool running = true;

int handleEvents(void) {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT: running = false; break;
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
					case SDLK_BACKSPACE: return bkspChar;
					case SDLK_ESCAPE: return SDLK_ESCAPE;
					case SDLK_RETURN: return SDLK_RETURN;
					case SDLK_TAB: return SDLK_TAB;
					case SDLK_UP: return SDLK_UP;
					case SDLK_DOWN: return SDLK_DOWN;
				}
				break;
			case SDL_TEXTINPUT: {
				return event.text.text[0];
			}
		}
	}
	return '\0';
}

int curFrame = 0;
timestamp ts_oldFrameStart={0,0},ts_newFrameStart={0,0},ts_frameDelta={0,0};
#ifdef LOG_TIMING_TO
timestamp ts_compTime = {0,0}, ts_now = {0,0};
#endif

void frameLoop(void) {
	clock_gettime(CLOCK_MONOTONIC, &ts_newFrameStart);
	for (; running; curFrame++) {
		ts_oldFrameStart = ts_newFrameStart;
		clock_gettime(CLOCK_MONOTONIC, &ts_newFrameStart);
		getTimeDelta(&ts_oldFrameStart, &ts_newFrameStart, &ts_frameDelta);
		#ifdef LOG_TIMING_TO
		fprintf(LOG_TIMING_TO,
			"ts_frameDelta: %1ld s, %9ld ns\n",
			ts_frameDelta.tv_sec, ts_frameDelta.tv_nsec
		);
		#endif
		int charEntered = handleEvents();
		if (!running) break;
		switch (whereAreWe) {
			case screen_mainMenu:
				whereAreWe = mainMenuLoop(charEntered, curFrame);
				break;
			case screen_glyphReactor:
				whereAreWe = glyphReactorLoop(charEntered, curFrame);
				break;
			case screen_spiroViewer:
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
}
