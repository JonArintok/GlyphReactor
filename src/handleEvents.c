
#include "handleEvents.h"
#include "initWindow.h"
#include "initOpenGl.h"
#include "../img/texAtlas.h"

int whereAreWe = spirographEditor;

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
					case SDLK_ESCAPE:
						switch (whereAreWe) {
							case mainMenu: return false; break;
							case glyphReactor:     whereAreWe = mainMenu; break;
							case spirographEditor: whereAreWe = mainMenu; break;
						}
						break;
					case SDLK_RETURN:
						switch (whereAreWe) {
							case mainMenu: whereAreWe = glyphReactor; break;
							case glyphReactor: break;
							case spirographEditor: break;
						}
						break;
					case SDLK_TAB:
						switch (whereAreWe) {
							case mainMenu:
								whereAreWe = spirographEditor;
								glUniform2f(unif_translate, 0, 0);
								break;
							case glyphReactor: break;
							case spirographEditor: whereAreWe = mainMenu; break;
						}
						break;
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
