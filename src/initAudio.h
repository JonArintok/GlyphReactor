
#include "optionsAndErrors.h"

enum {
	voice_spiro0,
	voice_typo = visSpirosSize,
	voice_bksp,
	voice_misbksp,
	voice_menuUp,
	voice_menuDn,
	voice_menuSelect,
	voice_menuEnd,
	voice_COUNT
};

enum {
	shape_default, // {1.0}
	#define shape_default_len 1
	shape_sine,
	#define shape_sine_len 1024
	shape_saw,
	#define shape_saw_len 1024
	shape_tri,
	#define shape_tri_len 1024
	shape_halfPulse,
	#define shape_halfPulse_len 2
	shape_quartPulse,
	#define shape_quartPulse_len 4
	shape_eighthPulse,
	#define shape_eighthPulse_len 8
	shape_COUNT
};

#define originPitch 36.0

void initAudio(void);
void cleanupAudio(void);
