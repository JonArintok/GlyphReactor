
#include <stdio.h>
#include <SDL2/SDL.h>

#include "initWindow.h"
#include "initOpenGl.h"
#include "initSprites.h"
#include "glyphReactorLoop.h"
#include "../img/texAtlas.h"
#include "initAudio.h"
#include "libVoice/voice.h"

int listPos = 0;
int listMoveDir = 0;
#define listMoveTime 20 // frames
int frameWhenListMoved = -listMoveTime;
#define originX (-videoW/4)

int mainMenuLoop(int charEntered, int curFrame) {
	switch (charEntered) {
		case SDLK_ESCAPE: return screen_quitGame;
		case SDLK_RETURN:
			initGlyphReactorLoop(initWordQueueSprites(listPos));
			return screen_glyphReactor;
		case SDLK_TAB:
			glUniform2f(unif_translate, 0, 0);
			return screen_spiroViewer;
		case SDLK_UP: // fall
		case 'u':
			if (listPos >= courseCount-1) {
				restartVoice(voice_menuEnd);
				break;
			}
			listPos++;
			listMoveDir = 1;
			frameWhenListMoved = curFrame;
			restartVoice(voice_menuUp);
			break;
		case SDLK_DOWN: // fall
		case 'd':
			if (!listPos) break;
			listPos--;
			listMoveDir = -1;
			frameWhenListMoved = curFrame;
			restartVoice(voice_menuDn);
			break;
	}
	// draw course list
	const float listMovePhase = (float)(curFrame-frameWhenListMoved)/listMoveTime;
	float cursorPosY = txtOriginY_ + texAtlGlyphH*listPos;
	if (listMovePhase < 1.0) {
		cursorPosY += pow(listMovePhase, 0.5)*listMoveDir*texAtlGlyphH - listMoveDir*texAtlGlyphH;
	}
	glUniform2f(unif_translate, originX, cursorPosY);
	glDrawArrays(GL_POINTS, charVertBeg, fileNamesCharCount);
	// draw cursor
	glUniform2f(unif_translate, originX, txtOriginY_);
	glDrawArrays(GL_POINTS, menuCursorVertBeg, menuCursorSpritesSize);
	return screen_mainMenu;
}
