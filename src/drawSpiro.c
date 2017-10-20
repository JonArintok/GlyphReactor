

#include "misc.h"
#include "initWindow.h"
#include "initOpenGl.h"
#include "initSprites.h"
#include "fromHue.h"
#include "../img/texAtlas.h"
#include "libVoice/voice.h"
#include "initAudio.h"

enum { // letters sorted from most to least used (according to wikipedia)
	arank_E,
	arank_T,
	arank_A,
	arank_O,
	arank_I,
	arank_N,
	arank_S,
	arank_H,
	arank_R,
	arank_D,
	arank_L,
	arank_C,
	arank_U,
	arank_M,
	arank_W,
	arank_F,
	arank_G,
	arank_Y,
	arank_P,
	arank_B,
	arank_V,
	arank_K,
	arank_J,
	arank_X,
	arank_Q,
	arank_Z,
	alphabetLength
};
const int alphaRanks[alphabetLength] = { // ascii order
	arank_A, arank_B, arank_C, arank_D, arank_E, arank_F, arank_G,
	arank_H, arank_I, arank_J, arank_K, arank_L, arank_M, arank_N, arank_O, arank_P,
	arank_Q, arank_R, arank_S,
	arank_T, arank_U, arank_V,
	arank_W, arank_X,
	arank_Y, /* and */ arank_Z
};
const float alphaIntervals[alphabetLength] = { // scale steps up from the origin
	2, 4, 5, 1,
	7, 9, 11, 12, 8,
	14, 16, 18, 19, 15,
	21, 23, 25, 26, 22,
	3, 10, 17, 24,
	6, 13, 20
}; // arranged to sound "mostly pentatonic"
#define scaleStepCount 7
const float scaleIntervals[scaleStepCount] = { // each scale step's distance in pitch from the tonic
	0, 2, 4, 5, 7, 9, 11
}; // major scale
double pitchFromScaleStep(int interval) {
	return originPitch + 12*(interval/scaleStepCount) + scaleIntervals[interval%scaleStepCount];
}
double incFromScaleStep(long shapeLength, int scaleStep) {
	return incFromFreq(shapeLength, freqFromPitch(pitchFromScaleStep(scaleStep)));
}
int scaleStepFromAlphaStep(int alphaStep) {
	return alphaIntervals[alphaRanks[alphaStep]];
}

void setSpiroVoice(int i, char c) {
	voice v = {
		// shape,         amp, sft, pos, inc
		{  shape_sine,    1.0, 0.0, 0.0, 0.0 }, // wave
		{  shape_default, 1.0, 0.0, 0.0, 0.0 }, // ampMod
		{  shape_default, 1.0, 0.0, 0.0, 0.0 }, // incMod
		{  shape_saw,     0.5, 0.5, 0.0, incFromPeriod(4.0)}, // ampEnv
		{  shape_default, 0.5, 0.5, 0.0, 0.0 }  // incEnv
	};
	if (c == ' ') {
		v[vo_wave].inc = incFromFreq(shape_sine_len, freqFromPitch(originPitch));
		v[vo_wave].amp = 1.4;
		v[vo_ampMod].shape = shape_sine;
		v[vo_ampMod].inc = v[vo_wave].inc/2.0;
		v[vo_ampMod].amp = 0.5;
		v[vo_ampMod].shift = 1.5;
	}
	else if (c >= 'a' && c <= 'z') {
		v[vo_wave].inc = incFromScaleStep(shape_sine_len, scaleStepFromAlphaStep(c - 'a'));
		v[vo_wave].amp = 1.0 - (v[vo_wave].inc/0.06); // decrease the denominator to increase the amount of drop-off as pitch rises
		v[vo_ampMod].shape = shape_sine;
		v[vo_ampMod].inc = v[vo_wave].inc*2.0;
		v[vo_ampMod].amp = v[vo_wave].amp*0.7;
	}
	else if (c >= 'A' && c <= 'Z') {
		v[vo_wave].inc = incFromScaleStep(shape_sine_len, scaleStepFromAlphaStep(c - 'A'));
		v[vo_wave].amp = 1.0 - (v[vo_wave].inc/0.06);
		v[vo_ampMod].shape = shape_sine;
		v[vo_ampMod].inc = v[vo_wave].inc/2.0;
		v[vo_ampMod].amp = v[vo_wave].amp*0.8;
	}
	else if (c >= '0' && c <= '9') {
		v[vo_wave].shape = shape_tri;
		v[vo_wave].inc = incFromScaleStep(shape_tri_len, c - '0');
	}
	else {
		v[vo_wave].inc = incFromScaleStep(shape_sine_len, alphabetLength+1);
		v[vo_wave].amp = 1.0 - (v[vo_wave].inc/0.06);
	}
	setVoice(voice_spiro0+i, v);
}

char visSpiroGlyphs[visSpirosSize];

void addSpiro(int i, char c) {
	visSpiroGlyphs[i] = c;
	const int glyphi = c-texAtlGlyphsAsciiStart;
	visSpiros[i] = glyphSpiros[glyphi];
	visSpiros[i].exploPhase = 0;
	setSpiroVoice(i, c);
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
					setColorFromPhase(&spiroSprites[spriteIndex], vs->exploPhase, charHue);
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
		fr (arm, spiroArmCount) {
			vs->offsets[arm] += vs->offsetVelocs[arm];
		}
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
