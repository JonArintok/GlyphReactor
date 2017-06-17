
#include "initWindow.h"
#include "initSprites.h"
#include "handleEvents.h"
#include "mainMenuLoop.h"
#include "glyphReactorLoop.h"
#include "spiroViewerLoop.h"
#include "optionsAndErrors.h"
#include "timestamp.h"

int curFrame = 0;
timestamp ts_oldFrameStart={0,0},ts_newFrameStart={0,0},ts_frameDelta={0,0};
#ifdef LOG_TIMING_TO
timestamp ts_compTime = {0,0}, ts_now = {0,0};
#endif

void frameLoop(void) {
	initGlyphReactorLoop();
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
				running = mainMenuLoop(charEntered, curFrame);
				break;
			case glyphReactor:
				running = glyphReactorLoop(charEntered, curFrame);
				break;
			case spirographEditor:
				running = spirographEditorLoop(charEntered, curFrame);
				break;
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
	free(chars);       // malloc in "initSprites.c"
	free(gunSprites);  // malloc in "initSprites.c"
	free(charSprites); // malloc in "initSprites.c"
	free(beamSprites); // malloc in "initSprites.c"
	SDL_GL_DeleteContext(GLcontext);_sdlec // init int "initWindow.c"
	SDL_Quit();_sdlec // init int "initWindow.c"
}
