

#include "misc.h"
#include "initWindow.h"
#include "initOpenGl.h"
#include "initSprites.h"
#include "fromHue.h"
#include "../img/texAtlas.h"

char visSpiroGlyphs[visSpirosSize];

// returns -1 if no rool in visSpiroGlyphs
int triggerSpiro(char glyph) {
	int i = 0;
	for (; i < visSpirosSize; i++) {
		if (visSpiros[i].exploPhase < 0) {
			visSpiroGlyphs[i] = glyph;
			visSpiros[i] = glyphSpiros[glyph-texAtlGlyphsAsciiStart];
			visSpiros[i].exploPhase = 0;
			break;
		}
	}
	return i == visSpirosSize ? -1 : i;
}


void drawSpiros(void) {
	int spriteIndex = 0;
	fr (i, visSpirosSize) {
		spirograph *const vs = &visSpiros[i];
		if (vs->exploPhase < 0) continue;
		fr (tick, vs->ticksPerFrame) {
			const double tickPhase = (double)tick/vs->ticksPerFrame;
			fr (arm, spiroArmCount) {
				if (!vs->arms[arm].armLength) break;
				vs->arms[arm].posX =
					(arm ? vs->arms[arm-1].posX : 0) + vs->arms[arm].armLength
					* sinTau(tickPhase*vs->arms[arm].revsWithinFrame + vs->offsets[arm])
				;
				vs->arms[arm].posY =
					(arm ? vs->arms[arm-1].posY : 0) + vs->arms[arm].armLength
					* sinTau(tickPhase*vs->arms[arm].revsWithinFrame + vs->offsets[arm] + 0.25)
				;
				if (vs->stampEnablePerArm & 1 << arm) {
					spiroSprites[spriteIndex].dstCX = vs->arms[arm].posX;
					spiroSprites[spriteIndex].dstCY = vs->arms[arm].posY;
					spiroSprites[spriteIndex].dstHW = texAtlGlyphW/2.0;
					spiroSprites[spriteIndex].dstHH = texAtlGlyphH/2.0;
					spiroSprites[spriteIndex].srcX  = texAtlGlyphPosX(visSpiroGlyphs[i]);
					spiroSprites[spriteIndex].srcY  = texAtlGlyphPosY(visSpiroGlyphs[i]);
					spiroSprites[spriteIndex].srcW  = texAtlGlyphW;
					spiroSprites[spriteIndex].srcH  = texAtlGlyphH;
					spiroSprites[spriteIndex].mulR  = UINT16_MAX * redFromHue((double)arm/spiroArmCount);
					spiroSprites[spriteIndex].mulG  = UINT16_MAX * grnFromHue((double)arm/spiroArmCount);
					spiroSprites[spriteIndex].mulB  = UINT16_MAX * bluFromHue((double)arm/spiroArmCount);
					spiroSprites[spriteIndex].mulO  = UINT16_MAX;
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
