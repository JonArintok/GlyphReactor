
#include "misc.h"
#include "initAudio.h"
#include "libVoice/voice.h"


enum {
	shape_default, // [1] {1.0}
	shape_sine,
	shape_saw,
	shape_halfPulse,
	shape_quartPulse,
	shape_eighthPulse,
	shape_COUNT
};

void initAudio(void) {
	initVoices(voice_COUNT, shape_COUNT);
	shapeFromSine(shape_sine, 1024);
	shapeFromSaw (shape_saw,  1024);
	shapeFromPulse(shape_halfPulse,   2, 1/2);
	shapeFromPulse(shape_quartPulse,  4, 1/4);
	shapeFromPulse(shape_eighthPulse, 8, 1/8);
	voice v = {
		// shape,         amp, shift, pos, inc
		{ -shape_sine,    1.0, 0.0,   1.0, 0.0 }, // wave
		{  shape_default, 1.0, 0.0,   1.0, 0.0 }, // ampMod
		{  shape_default, 1.0, 0.0,   1.0, 0.0 }, // incMod
		{  shape_saw,     0.5, 0.5,   1.0, 0.0 }, // ampEnv
		{  shape_default, 0.5, 0.5,   1.0, 0.0 }  // incEnv
	};
	
	// long sine wave, pitch determined by glyph
	fr (i, visSpirosSize) {
		setVoice(voice_spiro0+i, v);
		//setOscIncFromFreq(voice_spiro0+i, vo_wave, 440.0);
		setOscIncFromPeriod(voice_spiro0+i, vo_ampEnv, 2.0);
	}
	
	// short, metallic, frequency modulated sine wave, pitch determined by glyph
	setVoice(voice_typo, v);
	//setOscIncFromFreq(voice_typo, vo_wave,   440.0);
	//setOscIncFromFreq(voice_typo, vo_incMod, 440.0*0.7);
	setOscShape(voice_typo, vo_incMod, shape_sine);
	setOscIncFromPeriod(voice_typo, vo_ampEnv, 0.3);
	
	// short saw wave, pitch rise to pitch from glyph
	setVoice(voice_bksp, v);
	//setOscIncFromFreq(voice_bksp, vo_wave, 440.0);
	setOscShape(voice_bksp, vo_incEnv, shape_saw);
	setOscAmp(voice_bksp, vo_ampMod, -1.0);
	setOscIncFromPeriod(voice_bksp, vo_incEnv, 0.2);
	setOscIncFromPeriod(voice_bksp, vo_ampEnv, 0.6);
	
	// short eighth-pulse wave, square envelope
	setVoice(voice_misbksp, v);
	setOscShape(voice_misbksp, vo_wave, shape_eighthPulse);
	setOscIncFromFreq(voice_misbksp, vo_wave, freqFromPitch(36));
	setOscShape(voice_misbksp, vo_ampEnv, shape_halfPulse);
	setOscIncFromPeriod(voice_misbksp, vo_ampEnv, 0.6);
	
	// short sine wave, pitch rise
	setVoice(voice_menuUp, v);
	setOscIncFromFreq(voice_menuUp, vo_wave, freqFromPitch(60));
	setOscShape(voice_menuUp, vo_incEnv, shape_saw);
	setOscAmp(voice_menuUp, vo_ampMod, -1.0);
	setOscIncFromPeriod(voice_menuUp, vo_incEnv, 0.2);
	setOscIncFromPeriod(voice_menuUp, vo_ampEnv, 0.6);
	
	// short sine wave, pitch drop
	setVoice(voice_menuDn, v);
	setOscIncFromFreq(voice_menuDn, vo_wave, freqFromPitch(55));
	setOscShape(voice_menuDn, vo_incEnv, shape_saw);
	setOscIncFromPeriod(voice_menuDn, vo_incEnv, 0.2);
	setOscIncFromPeriod(voice_menuDn, vo_ampEnv, 0.6);
	
	// short sine wave with vibrato
	setVoice(voice_menuSelect, v);
	setOscIncFromFreq(voice_menuSelect, vo_wave, 440.0);
	setOscShape(voice_menuSelect, vo_incMod, shape_sine);
	setOscIncFromFreq(voice_menuSelect, vo_incMod, 6.0);
	setOscAmp(voice_menuSelect, vo_incMod, 0.02);
	setOscIncFromPeriod(voice_menuSelect, vo_ampEnv, 0.7);
	
	// short quart-pulse wave, square envelope
	setVoice(voice_menuEnd, v);
	setOscShape(voice_menuEnd, vo_wave, shape_quartPulse);
	setOscIncFromFreq(voice_menuEnd, vo_wave, 200.0);
	setOscShape(voice_menuEnd, vo_ampEnv, shape_quartPulse);
	setOscIncFromPeriod(voice_menuEnd, vo_ampEnv, 0.6);
	
	enableVoices(firstMainMenuLoopVoice, lastMainMenuLoopVoice);
	unpauseAudio();
}

void cleanupAudio(void) {closeVoices();}
