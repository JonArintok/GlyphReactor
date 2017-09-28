
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
	const float phasedPos = pos + (movePhase < 1.0)*(pow(movePhase, 0.5)*-posMoveDir + posMoveDir);
	const float listTop = txtOriginY_ + texAtlGlyphH*courseCount/2;
	const float listPosY = listTop;
	const float cursorPosY = listTop - texAtlGlyphH*phasedPos;
	const float cursorCiel = videoHH_ - videoH/16;
	const float cursorVisiblizer = listTop < cursorCiel ? 1.0 : cursorCiel/listTop;
	const float listVisOffset = (listTop > cursorCiel)*(
		-(listTop-cursorCiel) + (listTop-cursorCiel)*2.0*phasedPos/courseCount
	);
	// draw course list
	glUniform2f(unif_translate, originX, listPosY+listVisOffset);
	glDrawArrays(GL_POINTS, charVertBeg, fileNamesCharCount);
	// draw cursor
	glUniform2f(unif_translate, originX, cursorPosY*cursorVisiblizer);
	glDrawArrays(GL_POINTS, menuCursorVertBeg, menuCursorSpritesSize);
	return screen_mainMenu;
}
