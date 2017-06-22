

#include "misc.h"
#include "initWindow.h"
#include "initOpenGl.h"
#include "initSprites.h"
#include "fromHue.h"
#include "../img/texAtlas.h"

char visSpiroGlyphs[visSpirosSize];

void addSpiro(int i, char glyph) {
	visSpiroGlyphs[i] = glyph;
	visSpiros[i] = glyphSpiros[glyph-texAtlGlyphsAsciiStart];
	visSpiros[i].exploPhase = 0;
}

// new takes spot of either first with exploPhase >= 1, or oldest
int triggerSpiro(char glyph) {
	int i = 0;
	float oldestPhase = 0.0;
	int   oldestIndex = 0;
	for (; i < visSpirosSize; i++) {
		if (visSpiros[i].exploPhase >= 1.0) {
			addSpiro(i, glyph);
			break;
		}
		if (visSpiros[i].exploPhase > oldestPhase) {
			oldestPhase = visSpiros[i].exploPhase;
			oldestIndex = i;
		}
	}
	if (i == visSpirosSize) {
		i = oldestIndex;
		addSpiro(i, glyph);
	}
	return i;
}


void drawSpiros(void) {
	int spriteIndex = 0;
	fr (i, visSpirosSize) {
		spirograph *const vs = &visSpiros[i];
		const double charHue = (double)(visSpiroGlyphs[i]-texAtlGlyphsAsciiStart)/texAtlGlyphsCount;
		if (vs->exploPhase >= 1) continue;
		fr (tick, vs->ticksPerFrame) {
			const double tickPhase = (double)tick/vs->ticksPerFrame;
			fr (arm, spiroArmCount) {
				if (!vs->arms[arm].armLength) break;
				vs->arms[arm].posX = pow(vs->exploPhase, 0.2) * ( (arm ? vs->arms[arm-1].posX : 0) + vs->arms[arm].armLength * sinTau(tickPhase*vs->arms[arm].revsWithinFrame + vs->offsets[arm]) );
				vs->arms[arm].posY = pow(vs->exploPhase, 0.2) * ( (arm ? vs->arms[arm-1].posY : 0) + vs->arms[arm].armLength * sinTau(tickPhase*vs->arms[arm].revsWithinFrame + vs->offsets[arm] + 0.25) );
				if (vs->stampEnablePerArm & 1 << arm) {
					spiroSprites[spriteIndex].dstCX = vs->arms[arm].posX;
					spiroSprites[spriteIndex].dstCY = vs->arms[arm].posY;
					spiroSprites[spriteIndex].dstHW = texAtlGlyphW/2.0;
					spiroSprites[spriteIndex].dstHH = texAtlGlyphH/2.0;
					spiroSprites[spriteIndex].srcX  = texAtlGlyphPosX(visSpiroGlyphs[i]);
					spiroSprites[spriteIndex].srcY  = texAtlGlyphPosY(visSpiroGlyphs[i]);
					spiroSprites[spriteIndex].srcW  = texAtlGlyphW;
					spiroSprites[spriteIndex].srcH  = texAtlGlyphH;
					spiroSprites[spriteIndex].mulR  = 0xff * (0.4 + 0.6*redFromHue(charHue));
					spiroSprites[spriteIndex].mulG  = 0xff * (0.4 + 0.6*grnFromHue(charHue));
					spiroSprites[spriteIndex].mulB  = 0xff * (0.4 + 0.6*bluFromHue(charHue));
					spiroSprites[spriteIndex].mulO  = 0xff;
					spiroSprites[spriteIndex].rot   = tickPhase*vs->arms[arm].glyphRevsWithinFrame;
					spriteIndex++;
					if (spriteIndex == spiroSpritesSize) {
						printf("WARNING: spriteIndex has hit spiroSpritesSize(%i)\n", spiroSpritesSize);
						goto draw;
					}
				}
			}
		}
		vs->exploPhase += spiroExploSpeed;
		fr (arm, spiroArmCount) {
			vs->offsets[arm] += vs->offsetVelocs[arm];
		}
	}
	draw:
	#ifdef LOG_VERTEX_DATA_TO
	fprintf(LOG_VERTEX_DATA_TO, "\nvisSpiros\n");
	printSprites(spiroSprites, spriteIndex, __LINE__);
	#endif
	glBufferSubData(
		GL_ARRAY_BUFFER,             // GLenum        target
		spiroVertBeg*sizeof(sprite), // GLintptr      offset
		spriteIndex*sizeof(sprite),  // GLsizeiptr    size
		(const GLvoid*)spiroSprites  // const GLvoid *data
	);
	glDrawArrays(GL_POINTS, spiroVertBeg, spriteIndex);
}
