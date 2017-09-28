
#include <stdio.h>
#include <SDL2/SDL.h>

#include "initWindow.h"
#include "initOpenGl.h"
#include "initSprites.h"
#include "glyphReactorLoop.h"
#include "../img/texAtlas.h"
#include "initAudio.h"
#include "libVoice/voice.h"
#include "misc.h"

int pos = 0;
int posMoveDir = 0;
#define listMoveTime 20 // frames
int frameWhenListMoved = -listMoveTime;
#define originX (-videoW/4)

int mainMenuLoop(int charEntered, int curFrame) {
	switch (charEntered) {
		case SDLK_ESCAPE: return screen_quitGame;
		case SDLK_RETURN:
			restartVoice(voice_menuSelect);
			initGlyphReactorLoop(initWordQueueSprites(pos));
			return screen_glyphReactor;
		case SDLK_TAB:
			glUniform2f(unif_translate, 0, 0);
			return screen_spiroViewer;
		case SDLK_UP: // fall
		case 'u':
			if (!pos) {restartVoice(voice_menuEnd); break;}
			pos--;
			posMoveDir = 1;
			frameWhenListMoved = curFrame;
			restartVoice(voice_menuUp);
			break;
		case SDLK_DOWN: // fall
		case 'd':
			if (pos >= courseCount-1) {restartVoice(voice_menuEnd); break;}
			pos++;
			posMoveDir = -1;
			frameWhenListMoved = curFrame;
			restartVoice(voice_menuDn);
			break;
	}
	const float movePhase = (float)(curFrame-frameWhenListMoved)/listMoveTime;
	// draw course list
	float listPosY = txtOriginY_ - courseCount/2;
	glUniform2f(unif_translate, originX, listPosY);
	glDrawArrays(GL_POINTS, charVertBeg, fileNamesCharCount);
	// draw cursor
	float cursorPosY = txtOriginY_ - texAtlGlyphH*pos;
	if (movePhase < 1.0) {
		cursorPosY += pow(movePhase, 0.5)*posMoveDir*texAtlGlyphH - posMoveDir*texAtlGlyphH;
	}
	glUniform2f(unif_translate, originX, cursorPosY);
	glDrawArrays(GL_POINTS, menuCursorVertBeg, menuCursorSpritesSize);
	return screen_mainMenu;
}
