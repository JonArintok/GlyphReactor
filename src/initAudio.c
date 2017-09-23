
#include "libVoice/voice.h"
#include "initAudio.h"

enum {
	shape_sineWav,
	shape_sawWav,
	shape_squareWav,
	shape_halfPulseWav,
	shape_quartPulseWav,
	shape_COUNT
};

void initAudio(void) {
	initVoices(voice_COUNT, shape_COUNT);
	shapeFromSine (shape_sineWav, 1024);
	shapeFromSaw  (shape_sawWav,  1024);
	shapeFromPulse(shape_squareWav,     2, 0.5);
	shapeFromPulse(shape_halfPulseWav,  4, 0.25);
	shapeFromPulse(shape_quartPulseWav, 8, 0.125);
}

void cleanupAudio(void) {
	
}
