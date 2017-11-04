

#include "misc.h"
#include "initWindow.h"
#include "initOpenGl.h"
#include "initSprites.h"
#include "fromHue.h"
#include "../img/texAtlas.h"
#include "libVoice/voice.h"
#include "initAudio.h"


char visSpiroGlyphs[visSpirosSize];

void addSpiro(int i, char c) {
	visSpiroGlyphs[i] = c;
	const int glyphi = c-texAtlGlyphsAsciiStart;
	visSpiros[i] = glyphSpiros[glyphi];
	visSpiros[i].exploPhase = 0;
	setGlyphVoice(voice_spiro0+i, c, true);
}
// new takes spot of either first with exploPhase >= 1, or oldest
int triggerSpiro(char c) {
	float oldestPhase = 0.0;
	int   oldestIndex = 0;
	fr (i, visSpirosSize) {
		if (visSpiros[i].exploPhase >= 1.0) {
			addSpiro(i, c);
			return i;
		}
		if (visSpiros[i].exploPhase > oldestPhase) {
			oldestPhase = visSpiros[i].exploPhase;
			oldestIndex = i;
		}
	}
	addSpiro(oldestIndex, c);
	return oldestIndex;
}

void clearSpiros(void) {
	fr (i, visSpirosSize) visSpiros[i].exploPhase = 1.0;
}

void drawSpiros(void) {
	int spriteIndex = 0;
	fr (i, visSpirosSize) {
		spirograph *const vs = &visSpiros[i];
		const double charHue = hueFromChar(visSpiroGlyphs[i]);
		if (vs->exploPhase >= 1) continue;
		fr (tick, vs->ticksPerFrame) {
			const double tickPhase = (double)tick/vs->ticksPerFrame;
			const double phaseExponent = 0.1;
			fr (arm, spiroArmCount) {
				if (!vs->arms[arm].armLength) break;
				vs->arms[arm].posX =
					pow(vs->exploPhase, phaseExponent) *
					(
						(arm ? vs->arms[arm-1].posX : 0) +
						vs->arms[arm].armLength * sinTau(
							tickPhase*vs->arms[arm].revsWithinFrame
						)
					)
				;
				vs->arms[arm].posY =
					pow(vs->exploPhase, phaseExponent) * (
						(arm ? vs->arms[arm-1].posY : 0) +
						vs->arms[arm].armLength * sinTau(
							tickPhase*vs->arms[arm].revsWithinFrame + 0.25
						)
					)
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
					setHueFromPhase(&spiroSprites[spriteIndex], vs->exploPhase, charHue);
					spiroSprites[spriteIndex].rot = tickPhase*vs->arms[arm].glyphRevsWithinFrame;
					spriteIndex++;
					if (spriteIndex >= spiroSpritesSize) {
						printf("WARNING: spriteIndex has hit spiroSpritesSize(%i)\n", spiroSpritesSize);
						goto draw;
					}
				}
			}
		}
		vs->exploPhase += spiroExploSpeed;
	}
	draw:
	#ifdef LOG_VERTEX_DATA_TO
	//fprintf(LOG_VERTEX_DATA_TO, "\nvisSpiros\n");
	//printSprites(spiroSprites, spriteIndex, __LINE__);
	#endif
	glBufferSubData(
		GL_ARRAY_BUFFER,             // GLenum        target
		spiroVertBeg*sizeof(sprite), // GLintptr      offset
		spriteIndex*sizeof(sprite),  // GLsizeiptr    size
		(const GLvoid*)spiroSprites  // const GLvoid *data
	);
	glDrawArrays(GL_POINTS, spiroVertBeg, spriteIndex);
}
