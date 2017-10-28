
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
#include "libVoice/voice.h"
#include "initAudio.h"


int queueCharCount; // number of characters from file
int visCharBeg; // starts at gunDistance, goes up and down
int visCharEnd; // gunDistance + queueCharCount
#define visCharCount_ (visCharEnd - visCharBeg)
#define visCharVertBeg_ (charVertBeg + visCharBeg)

int stuckCharCount = 0;
int misBkspCount = 0;
int curWord = 0;
int frameWhenWordDropped = 0;
int frameWhenCharEntered;
int whereCurWordStarted;
int lastCharEntered = '\0';
double charHue;
const int beamGlowTime = 60; // frames
int gameOver = 0; // frame
const int postGameOverTime = 60; // frames

void initGlyphReactorLoop(int charCountIn) {
	queueCharCount = charCountIn;
	visCharBeg = gunDistance;
	visCharEnd = gunDistance + queueCharCount;
	frameWhenCharEntered = -beamGlowTime;
	whereCurWordStarted = visCharBeg;
}

float frand(float lb, float hb) {
	return lb + (hb - lb)*((float)rand()/RAND_MAX);
}

void setColorFlashFromPhase(sprite *s, double phase, double hue) {
	const double diff = pow(phase, 0.3); // curve to taste
	int rh = 0xff*redFromHue(hue);
	int gh = 0xff*grnFromHue(hue);
	int bh = 0xff*bluFromHue(hue);
	s->mulR = rh + (0xff-rh)*diff;
	s->mulG = gh + (0xff-gh)*diff;
	s->mulB = bh + (0xff-bh)*diff;
	s->mulO = 0xff;
}

int glyphReactorLoop(int charEntered, int curFrame) {
	// respond to character entered
	if (charEntered == SDLK_ESCAPE) {
		initMainMenuSprites();
		clearSpiros();
		return screen_mainMenu;
	}
	else if (!gameOver && charEntered && charEntered != SDLK_TAB && charEntered != SDLK_RETURN) {
		lastCharEntered = charEntered;
		frameWhenCharEntered = curFrame;
		charHue = hueFromChar(charEntered);
		if (charEntered == bkspChar) { // backspace
			const int bkspShape = shape_halfPulse;
			const double vwInc = incFromFreq(bkspShape, freqFromPitch(originPitch-12));
			if (stuckCharCount) {
				stuckCharCount--;
				visCharBeg++;
				voice v = { // short rising pitch from floor to pitch of deleted glyph
					// shape,         amp, sft, pos, inc
					{  bkspShape,     0.4, 0.0, 0.0, vwInc}, // wave
					{  shape_default, 1.0, 0.0, 0.0, 0.0 }, // ampMod
					{  shape_default, 1.0, 0.0, 0.0, 0.0 }, // incMod
					{  shape_saw,     0.5, 0.5, 0.0, incFromPeriod(0.4)}, // ampEnv
					{  shape_saw,    -1.0, 2.0, 0.0, incFromPeriod(0.3)}  // incEnv
				};
				setVoice(voice_bksp, v);
			}
			else {
				misBkspCount++;
				voice v = { // short tone
					// shape,         amp, sft, pos, inc
					{  bkspShape,     0.5, 0.0, 0.0, vwInc }, // wave
					{  shape_default, 1.0, 0.0, 0.0, 0.0 }, // ampMod
					{  shape_default, 1.0, 0.0, 0.0, 0.0 }, // incMod
					{  shape_saw,     0.5, 0.5, 0.0, incFromPeriod(0.2)}, // ampEnv
					{  shape_default, 0.5, 0.5, 0.0, 0.0 }  // incEnv
				};
				setVoice(voice_misbksp, v);
			}
		}
		else if (charEntered == chars[visCharBeg] && !stuckCharCount) { // correct
			triggerSpiro(charEntered);
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
		}
		else {
			if (stuckCharCount == gunDistance) gameOver = curFrame;
			else { // add stuck char
				setGlyphVoice(voice_typo, charEntered, false);
				visCharBeg--;
				stuckCharCount++;
				if (visCharBeg < 0 || visCharBeg >= visCharEnd) _SHOULD_NOT_BE_HERE_;
				charSprites[visCharBeg] = charSprites[visCharBeg+1];
				charSprites[visCharBeg].dstCX -= texAtlGlyphW;
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
		// else ignore the keypress
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
			setHueFromPhase(&beamSprites[i], 0, charHue);
		}
		#ifdef LOG_VERTEX_DATA_TO
		//fprintf(LOG_VERTEX_DATA_TO, "\nBEAM\n");
		//printSprites(beamSprites, beamSize, __LINE__);
		#endif
		setHueFromPhase(&gunSprites[0], 0, charHue);
	}
	// draw beam
	glUniform2f(unif_translate, txtOriginX_, txtOriginY_);
	const float beamPhase = (float)(curFrame-frameWhenCharEntered)/beamGlowTime;
	if (beamPhase < 1) {
		fr (i, beamSize) {
			setHueFromPhase(&beamSprites[i], beamPhase, charHue);
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
	if (beamPhase <= 1) {
		if (gameOver == curFrame) {
			const uint8_t mulCol = 0x20;
			gunSprites[1].mulR = mulCol;
			gunSprites[1].mulG = mulCol;
			gunSprites[1].mulB = mulCol;
		}
		else if (!gameOver) {
			setColorFlashFromPhase(&gunSprites[1], beamPhase, charHue);
		}
		if (gameOver <= curFrame) {
			glBufferSubData(
				GL_ARRAY_BUFFER,               // GLenum        target
				sizeof(sprite)*(gunVertBeg+1), // GLintptr      offset
				sizeof(sprite)*1,              // GLsizeiptr    size
				(const GLvoid*)&gunSprites[1]  // const GLvoid *data
			);
		}
	}
	glDrawArrays(GL_POINTS, gunVertBeg, gunSpritesSize);
	// draw spirographs
	glUniform2f(unif_translate, txtOriginX_, txtOriginY_);
	drawSpiros();
	// draw word queue and stuck chars
	if (gameOver) {
		const float gameOverPhase = (float)(curFrame-gameOver)/postGameOverTime;
		if (gameOverPhase < 1) {
			if (gameOver == curFrame) restartVoice(voice_gameOver);
			const float affectedCount = gunDistance + maxWordSize/2;
			const float throw = videoW/8.0; // to taste
			fr (i, affectedCount) {
				charSprites[visCharBeg+i].dstCX += (1.0-pow(gameOverPhase, 0.3))*(1.0-pow(i/affectedCount, 0.1))*throw;
			}
			glBufferSubData(
				GL_ARRAY_BUFFER,                        // GLenum        target
				sizeof(sprite)*visCharVertBeg_,         // GLintptr      offset
				sizeof(sprite)*affectedCount,           // GLsizeiptr    size
				(const GLvoid*)&charSprites[visCharBeg] // const GLvoid *data
			);
		}
	}
	else if (stuckCharCount >= gunDistance-2) {
		// stuck characters jiggle
		const float jiggleRange = texAtlGlyphW/(2*(gunDistance-stuckCharCount+1));
		fr (i, stuckCharCount) {
			charSprites[visCharBeg+i].dstCY = txtOriginY_ - curWord*texAtlGlyphH + frand(-jiggleRange, jiggleRange);
		}
		glBufferSubData(
			GL_ARRAY_BUFFER,                        // GLenum        target
			sizeof(sprite)*visCharVertBeg_,         // GLintptr      offset
			sizeof(sprite)*stuckCharCount,          // GLsizeiptr    size
			(const GLvoid*)&charSprites[visCharBeg] // const GLvoid *data
		);
	}
	glUniform2f(
		unif_translate,
		txtOriginX_,
		txtOriginY_ + curWord*texAtlGlyphH - bouncePos((double)(curFrame-frameWhenWordDropped)/beamGlowTime)
	);
	glDrawArrays(GL_POINTS, visCharVertBeg_, visCharCount_);
	return screen_glyphReactor;
}
