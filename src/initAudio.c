
#include "misc.h"
#include "initAudio.h"
#include "libVoice/voice.h"


void initAudio(void) {
	initVoices(voice_COUNT, shape_COUNT);
	shapeFromSine (shape_sine, shape_sine_len);
	shapeFromSaw  (shape_saw,  shape_saw_len);
	shapeFromTri  (shape_tri,  shape_tri_len);
	shapeFromPulse(shape_halfPulse,   shape_halfPulse_len,   1.0/2.0);
	shapeFromPulse(shape_quartPulse,  shape_quartPulse_len,  1.0/4.0);
	shapeFromPulse(shape_eighthPulse, shape_eighthPulse_len, 1.0/8.0);
	fr (i, visSpirosSize) {
		const voice v = { // spiro0+
			// shape,         amp, sft, pos, inc
			{  shape_sine,    1.0, 0.0, 1.0, 0.0 }, // wave
			{  shape_default, 1.0, 0.0, 1.0, 0.0 }, // ampMod
			{  shape_default, 1.0, 0.0, 1.0, 0.0 }, // incMod
			{  shape_saw,     0.5, 0.5, 1.0, incFromPeriod(4.0) }, // ampEnv
			{  shape_default, 0.5, 0.5, 1.0, 0.0 }  // incEnv
		};
		setVoice(voice_spiro0+i, v);
	}
	{ // typo
		const voice v = {
			// shape,         amp, sft, pos, inc
			{  shape_sine,    1.0, 0.0, 1.0, 0.0 }, // wave
			{  shape_default, 1.0, 0.0, 1.0, 0.0 }, // ampMod
			{  shape_default, 1.0, 0.0, 1.0, 0.0 }, // incMod
			{  shape_saw,     0.5, 0.5, 1.0, 0.0 }, // ampEnv
			{  shape_default, 0.5, 0.5, 1.0, 0.0 }  // incEnv
		};
		setVoice(voice_typo, v);
	}
	{ // bksp
		const voice v = {
			// shape,         amp, sft, pos, inc
			{  shape_sine,    1.0, 0.0, 1.0, 0.0 }, // wave
			{  shape_default, 1.0, 0.0, 1.0, 0.0 }, // ampMod
			{  shape_default, 1.0, 0.0, 1.0, 0.0 }, // incMod
			{  shape_saw,     0.5, 0.5, 1.0, 0.0 }, // ampEnv
			{  shape_default, 0.5, 0.5, 1.0, 0.0 }  // incEnv
		};
		setVoice(voice_bksp, v);
	}
	{ // misbksp
		const voice v = {
			// shape,         amp, sft, pos, inc
			{  shape_sine,    1.0, 0.0, 1.0, 0.0 }, // wave
			{  shape_default, 1.0, 0.0, 1.0, 0.0 }, // ampMod
			{  shape_default, 1.0, 0.0, 1.0, 0.0 }, // incMod
			{  shape_saw,     0.5, 0.5, 1.0, 0.0 }, // ampEnv
			{  shape_default, 0.5, 0.5, 1.0, 0.0 }  // incEnv
		};
		setVoice(voice_misbksp, v);
	}
	{ // game over
		const voice v = {
			// shape,         amp, sft, pos, inc
			{  shape_saw,     1.0, 0.0, 1.0, incFromFreq(shape_sine_len, freqFromPitch(originPitch)) },
			{  shape_default, 1.0, 0.0, 1.0, 0.0 }, // ampMod
			{  shape_default, 1.0, 0.0, 1.0, 0.0 }, // incMod
			{  shape_saw,     0.5, 0.5, 1.0, incFromPeriod(0.2) }, // ampEnv
			{  shape_saw,     8.0, 6.0, 1.0, incFromPeriod(0.2) }  // incEnv
		};
		setVoice(voice_gameOver, v);
	}
	{ // menu up
		const voice v = {
			// shape,         amp, sft, pos, inc
			{  shape_sine,    0.8, 0.0, 1.0, incFromFreq(shape_sine_len, freqFromPitch(55))}, // wave
			{  shape_default, 1.0, 0.0, 1.0, 0.0 }, // ampMod
			{  shape_default, 1.0, 0.0, 1.0, 0.0 }, // incMod
			{  shape_saw,     0.5, 0.5, 1.0, incFromPeriod(0.4)}, // ampEnv
			{  shape_saw,    -0.5, 1.5, 1.0, incFromPeriod(0.15)}  // incEnv
		};
		setVoice(voice_menuUp, v);
	}
	{ // menu dn
		const voice v = {
			// shape,         amp, sft, pos, inc
			{  shape_sine,    0.8, 0.0, 1.0, incFromFreq(shape_sine_len, freqFromPitch(64))}, // wave
			{  shape_default, 1.0, 0.0, 1.0, 0.0 }, // ampMod
			{  shape_default, 1.0, 0.0, 1.0, 0.0 }, // incMod
			{  shape_saw,     0.5, 0.5, 1.0, incFromPeriod(0.4)}, // ampEnv
			{  shape_saw,     0.5, 1.5, 1.0, incFromPeriod(0.15)}  // incEnv
		};
		setVoice(voice_menuDn, v);
	}
	{ // menu end
		const voice v = {
			// shape,            amp, sft, pos, inc
			{  shape_quartPulse, 1.0, 0.0, 1.0, incFromFreq(shape_quartPulse_len, 880)}, // wave
			{  shape_default,    0.4, 0.0, 1.0, 0.0 }, // ampMod
			{  shape_default,    1.0, 0.0, 1.0, 0.0 }, // incMod
			{  shape_saw,        0.5, 0.5, 1.0, incFromPeriod(0.05)}, // ampEnv
			{  shape_default,    0.5, 0.5, 1.0, 0.0}  // incEnv
		};
		setVoice(voice_menuEnd, v);
	}
	{ // menu select
		const voice v = {
			// shape,            amp, sft, pos, inc
			{  shape_sine,       1.0, 0.0, 1.0, incFromFreq(shape_sine_len, freqFromPitch(60))}, // wave
			{  shape_default,    0.4, 0.0, 1.0, 0.0 }, // ampMod
			{  shape_halfPulse, -0.5, 1.5, 1.0, incFromPeriod(0.1)}, // incMod
			{  shape_saw,        0.5, 0.5, 1.0, incFromPeriod(0.7)}, // ampEnv
			{  shape_default,    0.5, 0.5, 1.0, 0.0}  // incEnv
		};
		setVoice(voice_menuSelect, v);
	}
	setGlobalVolume(visSpirosSize/2.0);
	unpauseAudio();
}
void cleanupAudio(void) {closeVoices();}


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
double incFromScaleStep(long shapeLength, int scaleStep, double detunePitch) {
	return incFromFreq(shapeLength, freqFromPitch(pitchFromScaleStep(scaleStep)+detunePitch));
}
int scaleStepFromAlphaStep(int alphaStep) {
	return alphaIntervals[alphaRanks[alphaStep]];
}

void setGlyphVoice(int voiceIndex, char c, bool correct) {
	double vAmpEnvInc = correct ? incFromPeriod(4.0) : incFromPeriod(1.0);
	double detunePitch = frand(-0.07, 0.07);
	voice v = {
		// shape,         amp, sft, pos, inc
		{  shape_tri,     1.0, 0.0, 0.0, 0.0 }, // wave
		{  shape_default, 1.0, 0.0, 0.0, 0.0 }, // ampMod
		{  shape_default, 1.0, 0.0, 0.0, 0.0 }, // incMod
		{  shape_saw,     0.5, 0.5, 0.0, vAmpEnvInc}, // ampEnv
		{  shape_default, 0.5, 0.5, 0.0, 0.0 }  // incEnv
	};
	if (c == ' ') {
		v[vo_wave].inc = incFromFreq(shape_tri_len, freqFromPitch(originPitch+detunePitch));
		v[vo_wave].amp = 1.4;
		if (!correct) {
			v[vo_ampMod].shape = shape_halfPulse;
			v[vo_ampMod].inc =  incFromFreq(shape_halfPulse_len, freqFromPitch(originPitch+detunePitch+7));
			v[vo_ampMod].amp = v[vo_wave].amp*0.8;
		}
	}
	else if (c >= 'a' && c <= 'z') {
		v[vo_wave].inc = incFromScaleStep(shape_tri_len, scaleStepFromAlphaStep(c - 'a'), detunePitch);
		v[vo_wave].amp = 1.0 - (v[vo_wave].inc/0.06); // decrease the denominator to increase the amount of drop-off as pitch rises
		if (!correct) {
			v[vo_ampMod].shape = shape_halfPulse;
			v[vo_ampMod].inc = v[vo_wave].inc*0.7935; // arbitrary
			v[vo_ampMod].amp = v[vo_wave].amp*0.8;
		}
	}
	else if (c >= 'A' && c <= 'Z') {
		v[vo_wave].inc = incFromScaleStep(shape_tri_len, scaleStepFromAlphaStep(c - 'A'), detunePitch);
		v[vo_wave].amp = 1.0 - (v[vo_wave].inc/0.06);
		if (!correct) {
			v[vo_ampMod].shape = shape_halfPulse;
			v[vo_ampMod].inc = v[vo_wave].inc*0.7935; // arbitrary
			v[vo_ampMod].amp = v[vo_wave].amp*0.8;
		}
		else {
			v[vo_ampMod].shape = shape_tri;
			v[vo_ampMod].inc =  v[vo_wave].inc/2.0;
			v[vo_ampMod].amp = v[vo_wave].amp*0.8;
			v[vo_ampMod].shift = 0.8;
		}
	}
	else if (c >= '0' && c <= '9') {
		v[vo_wave].shape = shape_sine;
		v[vo_wave].inc = incFromScaleStep(shape_sine_len, c - '0', detunePitch);
		if (!correct) {
			v[vo_ampMod].shape = shape_halfPulse;
			v[vo_ampMod].inc = v[vo_wave].inc*0.7935; // arbitrary
			v[vo_ampMod].amp = v[vo_wave].amp*0.8;
		}
	}
	else {
		v[vo_wave].inc = incFromScaleStep(shape_sine_len, alphabetLength+2, detunePitch);
		v[vo_wave].amp = 1.0 - (v[vo_wave].inc/0.06);
		if (!correct) {
			v[vo_ampMod].shape = shape_halfPulse;
			v[vo_ampMod].inc = v[vo_wave].inc/0.7935; // arbitrary
			v[vo_ampMod].amp = v[vo_wave].amp*2.0;
		}
	}
	setVoice(voiceIndex, v);
}
