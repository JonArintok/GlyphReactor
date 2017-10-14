
#include <stdio.h>
#include "misc.h"
#include "glyphReactorLoop.h"
#include "initWindow.h"
#include "initSprites.h"
#include "initOpenGl.h"
#include "drawSpiro.h"
#include "cleanTxtFile.h"
#include "fromHue.h"
#include "initBounceEnv.h"
#include "../img/texAtlas.h"


int queueCharCount; // number of characters from file
int visCharBeg; // starts at gunDistance, goes up and down
int visCharEnd; // gunDistance + queueCharCount
#define visCharCount_ (visCharEnd - visCharBeg)
#define visCharVertBeg_ (charVertBeg + visCharBeg)

int stuckCharCount;
int misBkspCount;
int curWord;
int frameWhenWordDropped;
int frameWhenCharEntered;
int whereCurWordStarted;
int lastCharEntered = '\0';
double charHue;
const int beamGlowTime = 60; // frames


void initGlyphReactorLoop(int charCountIn) {
	queueCharCount = charCountIn;
	visCharBeg = gunDistance;
	visCharEnd = gunDistance + queueCharCount;
	stuckCharCount = 0;
	misBkspCount = 0;
	curWord = 0;
	frameWhenWordDropped = 0;
	frameWhenCharEntered = -beamGlowTime;
	whereCurWordStarted = visCharBeg;
}

int glyphReactorLoop(int charEntered, int curFrame) {
	// respond to character entered
	if (charEntered && charEntered != SDLK_TAB && charEntered != SDLK_RETURN) {
		if (charEntered == SDLK_ESCAPE) {
			initMainMenuSprites();
			clearSpiros();
			return screen_mainMenu;
		}
		lastCharEntered = charEntered;
		frameWhenCharEntered = curFrame;
		charHue = hueFromChar(charEntered);
		if (charEntered == bkspChar) { // backspace
			if (stuckCharCount) {
				stuckCharCount--;
				visCharBeg++;
			}
			else misBkspCount++;
		}
		else if (charEntered == chars[visCharBeg] && !stuckCharCount) { // correct
			visCharBeg++;
			if (charEntered == delim) {
				curWord++;
				whereCurWordStarted = visCharBeg;
				frameWhenWordDropped = curFrame;
			}
			else {
				int count = 0;
				for (int i = visCharBeg; i < charsSize; i++) {
					charSprites[i].dstCX -= texAtlGlyphW;
					count++;
					if (chars[i] == delim) break;
				}
				glBufferSubData(
					GL_ARRAY_BUFFER,                        // GLenum        target
					sizeof(sprite)*visCharVertBeg_,         // GLintptr      offset
					sizeof(sprite)*count,                   // GLsizeiptr    size
					(const GLvoid*)&charSprites[visCharBeg] // const GLvoid *data
				);
			}
			triggerSpiro(charEntered);
		}
		else { // incorrect
			visCharBeg--;
			stuckCharCount++;
			if (visCharBeg < 0 || visCharBeg >= visCharEnd) _SHOULD_NOT_BE_HERE_;
			charSprites[visCharBeg] = charSprites[visCharBeg+1];
			charSprites[visCharBeg].dstCX -= texAtlGlyphW;
			if (charSprites[visCharBeg].dstCX <=  0.0 - texAtlGlyphW*gunDistance) {
				return screen_gameOver;
			}
			else {
				chars[visCharBeg] = charEntered;
				charSprites[visCharBeg].dstHW = -texAtlGlyphW/2.0;
				charSprites[visCharBeg].dstHH = -texAtlGlyphH/2.0;
				charSprites[visCharBeg].srcX  = texAtlGlyphPosX(charEntered);
				charSprites[visCharBeg].srcY  = texAtlGlyphPosY(charEntered);
				charSprites[visCharBeg].mulR  = 0xff;
				charSprites[visCharBeg].mulG  = 0;
				charSprites[visCharBeg].mulB  = 0;
				charSprites[visCharBeg].mulO  = 0xff;
				glBufferSubData(
					GL_ARRAY_BUFFER,                        // GLenum        target
					sizeof(sprite)*visCharVertBeg_,         // GLintptr      offset
					sizeof(sprite),                         // GLsizeiptr    size
					(const GLvoid*)&charSprites[visCharBeg] // const GLvoid *data
				);
			}
		}
		// init beam
		const int boltCornerCount = gunDistance;
		const int boltDeviation = 32; // plus or minus
		int boltCorners[boltCornerCount];
		fr (i, boltCornerCount) {
			boltCorners[i] = rand()%(boltDeviation*2 + 1) - boltDeviation;
		}
		boltCorners[0] = 0;
		boltCorners[boltCornerCount-1] = 0;
		fr (i, beamSize) {
			const double phaseInBeam = (double)i/(beamSize-1);
			const double phaseInCorners = phaseInBeam*(boltCornerCount-1.0001); // get close to last element, but don't touch it
			beamSprites[i].dstCX = -gunDistance*texAtlGlyphW + (gunDistance-stuckCharCount)*phaseInBeam*texAtlGlyphW;
			beamSprites[i].dstCY = lerp(
				boltCorners[(int)phaseInCorners],
				boltCorners[(int)phaseInCorners+1],
				fractionalPart(phaseInCorners)
			);
			beamSprites[i].dstHW = texAtlGlyphW/2.0;
			beamSprites[i].dstHH = texAtlGlyphH/2.0;
			beamSprites[i].srcX  = texAtlGlyphPosX(charEntered);
			beamSprites[i].srcY  = texAtlGlyphPosY(charEntered);
			beamSprites[i].srcW  = texAtlGlyphW;
			beamSprites[i].srcH  = texAtlGlyphH;
			setColorFromPhase(&beamSprites[i], 0, charHue);
		}
		#ifdef LOG_VERTEX_DATA_TO
		//fprintf(LOG_VERTEX_DATA_TO, "\nBEAM\n");
		//printSprites(beamSprites, beamSize, __LINE__);
		#endif
	}
	// draw beam
	glUniform2f(unif_translate, txtOriginX_, txtOriginY_);
	const float beamPhase = (float)(curFrame-frameWhenCharEntered)/beamGlowTime;
	if (beamPhase < 1) {
		fr (i, beamSize) {
			setColorFromPhase(&beamSprites[i], beamPhase, charHue);
			beamSprites[i].mulO  = 0xff * (1.0 - pow(beamPhase, 0.5));
		}
		glBufferSubData(
			GL_ARRAY_BUFFER,             // GLenum        target
			sizeof(sprite)*beamVertBeg,  // GLintptr      offset
			sizeof(sprite)*beamSize,     // GLsizeiptr    size
			(const GLvoid*)beamSprites   // const GLvoid *data
		);
		glDrawArrays(GL_POINTS, beamVertBeg, beamSize);
	}
	// draw gun
	glDrawArrays(GL_POINTS, gunVertBeg, gunSpritesSize);
	// draw spirographs
	glUniform2f(unif_translate, txtOriginX_, txtOriginY_);
	drawSpiros();
	// draw word queue
	glUniform2f(
		unif_translate,
		txtOriginX_,
		txtOriginY_ + curWord*texAtlGlyphH - bouncePos((double)(curFrame-frameWhenWordDropped)/beamGlowTime)
	);
	glDrawArrays(GL_POINTS, visCharVertBeg_, visCharCount_);
	return screen_glyphReactor;
}
