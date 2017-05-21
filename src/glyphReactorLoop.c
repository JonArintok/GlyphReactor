
#include <stdio.h>
#include "misc.h"
#include "glyphReactorLoop.h"
#include "initWindow.h"
#include "initSprites.h"
#include "initOpenGl.h"
#include "cleanTxtFile.h"
#include "fromHue.h"
#include "initBounceEnv.h"
#include "../img/texAtlas.h"

int stuckCharCount = 0;
int misBkspCount = 0;
int curWord = 0;
int whereCurWordStarted;
int frameWhenWordDropped = 0;
const int beamGlowTime = 60; // frames
int frameWhenCharEntered;
int lastCharEntered = '\0';

void initGlyphReactorLoop(void) {
	frameWhenCharEntered = -beamGlowTime;
	whereCurWordStarted = visCharBeg;
}

bool glyphReactorLoop(char charEntered, int curFrame) {
	// respond to character entered
	if (charEntered) {
		lastCharEntered = charEntered;
		frameWhenCharEntered = curFrame;
		if (charEntered == bkspChar) {
			if (stuckCharCount) {
				stuckCharCount--;
				visCharBeg++;
			}
			else misBkspCount++;
		}
		else if (charEntered == chars[visCharBeg] && !stuckCharCount) {
			visCharBeg++;
			if (charEntered == delim) {
				curWord++;
				whereCurWordStarted = visCharBeg;
				frameWhenWordDropped = curFrame;
			}
		}
		else {
			visCharBeg--;
			stuckCharCount++;
			if (visCharBeg < 0 || visCharBeg >= visCharEnd) {
				return false;
			}
			charSprites[visCharBeg] = charSprites[visCharBeg+1];
			charSprites[visCharBeg].dstCX -= texAtlGlyphW;
			if (charSprites[visCharBeg].dstCX <=  0.0 - texAtlGlyphW*railLength) {
				return false;
			}
			else {
				chars[visCharBeg] = charEntered;
				charSprites[visCharBeg].dstHW = -texAtlGlyphW/2.0;
				charSprites[visCharBeg].dstHH = -texAtlGlyphH/2.0;
				charSprites[visCharBeg].srcX  = texAtlGlyphPosX(charEntered);
				charSprites[visCharBeg].srcY  = texAtlGlyphPosY(charEntered);
				charSprites[visCharBeg].mulR  = UINT16_MAX;
				charSprites[visCharBeg].mulG  = 0;
				charSprites[visCharBeg].mulB  = 0;
				charSprites[visCharBeg].mulO  = UINT16_MAX;
				glBufferSubData(
					GL_ARRAY_BUFFER,                        // GLenum        target
					visCharVertBeg_*sizeof(sprite),         // GLintptr      offset
					sizeof(sprite),                         // GLsizeiptr    size
					(const GLvoid*)&charSprites[visCharBeg] // const GLvoid *data
				);
			}
		}
	}
	// draw beam
	const float beamPhase = (float)(curFrame-frameWhenCharEntered)/beamGlowTime;
	if (beamPhase < 1) {
		const int beamSize = (railLength + (visCharBeg-whereCurWordStarted))*beamCharPerWidth;
		fr (i, beamSize) {
			beamSprites[i].dstCX = -railLength*texAtlGlyphW + i*texAtlGlyphW/beamCharPerWidth + sinTau(((curFrame-i)%12)/12.0)*1.8;
			beamSprites[i].dstCY = 0;
			beamSprites[i].dstHW = texAtlGlyphW/2.0;
			beamSprites[i].dstHH = texAtlGlyphH/2.0;
			beamSprites[i].srcX  = texAtlGlyphPosX(lastCharEntered);
			beamSprites[i].srcY  = texAtlGlyphPosY(lastCharEntered);
			beamSprites[i].srcW  = texAtlGlyphW;
			beamSprites[i].srcH  = texAtlGlyphH;
			const double huePhase = i/12.0 - beamPhase;
			beamSprites[i].mulR  = UINT16_MAX * redFromHue(huePhase);
			beamSprites[i].mulG  = UINT16_MAX * grnFromHue(huePhase);
			beamSprites[i].mulB  = UINT16_MAX * bluFromHue(huePhase);
			beamSprites[i].mulO  = UINT16_MAX * (1.0 - beamPhase);
		}
		#ifdef LOG_VERTEX_DATA_TO
		fprintf(LOG_VERTEX_DATA_TO, "\nBEAM\n");
		printSprites(beamSprites, beamSize, __LINE__);
		#endif
		glBufferSubData(
			GL_ARRAY_BUFFER,             // GLenum        target
			beamVertBeg*sizeof(sprite),  // GLintptr      offset
			beamSize*sizeof(sprite),     // GLsizeiptr    size
			(const GLvoid*)beamSprites   // const GLvoid *data
		);
		glUniform2f(unif_translate, txtOriginX_, txtOriginY_);
		glDrawArrays(GL_POINTS, 0, beamSize);
	}
	// draw word queue
	glUniform2f(
		unif_translate,
		txtOriginX_,
		txtOriginY_ + curWord*texAtlGlyphH - bouncePos((double)(curFrame-frameWhenWordDropped)/beamGlowTime)
	);
	glDrawArrays(GL_POINTS, visCharVertBeg_, visCharCount_);
	return true;
}
