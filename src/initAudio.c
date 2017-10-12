
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
	unpauseAudio();
}

void cleanupAudio(void) {closeVoices();}
