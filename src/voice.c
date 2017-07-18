// "frequency" or "freq" refers to a Hz value, and "pitch" refers to
// a numeric musical note with 0 representing C0, 12 for C1, etc..
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <SDL2/SDL.h>

#include "voice.h"
#include "misc.h"
#include "optionsAndErrors.h"

typedef struct {
	float    *shape;     // values are expected to be between -1 and 1
	uint64_t  shapeSize; // total count of samples(floats) in shape
	float     pos;       // position in shape
	float     spd;       // 1.0 results in 1 shape sample per output sample
	float     amp;       // multiply shape output sample
	uint16_t  interp;    // enumerated, how to interpolate between shape samples
	uint16_t  trip;      // enumerated, what to do when pos goes out of bounds
} envel;

typedef struct {
	envel wave;      // mono audio clip or waveform
	envel ampMod;    // modulates volume of wave (multiplied)
	envel spdMod;    // modulates speed of wave (multiplied)
	float pan;       // -1.0 is all the way left, 1.0 is all the way right
	bool  enable;    // voice will be ignored if this is false
} voice;

const double ChromaticRatio = 1.059463094359295264562; // the twelfth root of 2
const double A4freq  = 440.0;
const double A4pitch =  57.0;

double freqFromPitch(double pitch) {
	return pow(ChromaticRatio, pitch-A4pitch)*A4freq;
}

uint32_t sampleRate = 48000;
uint32_t frameRate = 60;
uint32_t floatStreamSize = 1024; // must be a power of 2

SDL_AudioDeviceID AudioDevice;
SDL_AudioSpec audioSpec;


void buildSineWave(float *data, uint32_t length) {
	fr (i, length) data[i] = sin(i*(tau/length));
}

#ifdef LOG_AUDIOSPEC_TO
void logSpec(SDL_AudioSpec as) {
	fprintf(LOG_AUDIOSPEC_TO,
		"freq    :%5d\n"
		"format  :%5d\n"
		"channels:%5d\n"
		"silence :%5d\n"
		"samples :%5d\n"
		"size    :%5d\n\n",
		as.freq,
		as.format,
		as.channels,
		as.silence,
		as.samples,
		as.size
	);
}
#endif

voice *voices = NULL;
int    voiceCount = 0;

void audioCallback(void *unused, uint8_t *byteStream, int byteStreamLength) {
	float *floatStream = (float*)byteStream;
	float enabledVoiceCount = 0;
	fr (v, voiceCount) {
		if (!voices[v].enable) continue;
		enabledVoiceCount++;
		double waveIncrem   = voices[v].wave.spd   / sampleRate;
		double ampModIncrem = voices[v].ampMod.spd / sampleRate;
		double spdModIncrem = voices[v].spdMod.spd / sampleRate;
		const double rightFactor = (voices[v].pan+1.0)/2;
		const double leftFactor  = 1.0 - rightFactor;
		double pos = voices[v].wave.pos;
		for (int s = 0; s < floatStreamSize; s += 2) {
			pos += waveIncrem;
			if (pos > 1 || pos < 0) {
				switch (voices[v].wave.trip) {
					case trip_once: goto nextVoice;
					case trip_loop:
						pos -= (long)pos;
						break;
					case trip_clamp:
						pos = pos > 1 ? 1.0 : 0.0;
						waveIncrem = 0;
						break;
					case trip_bounce:
						voices[v].wave.spd *= -1;
						waveIncrem *= -1;
						pos = pos > 1 ? 1.0 : 0.0;
						break;
					default: printf("ERROR: unrecognized 'trip' value for voice %i\n", v);
				}
			}
			
			double sample =
			floatStream[s  ] += sample * leftFactor;
			floatStream[s+1] += sample * rightFactor;
		}
		voices[v].wave.pos = pos;
		nextVoice:
	}
	fr (s, floatStreamSize) {
		floatStream[s] /= enabledVoiceCount;
	}
}

int initVoices(int initVoiceCount) {
	SDL_Init(SDL_INIT_AUDIO);_sdlec
	SDL_AudioSpec want = {0};
	want.freq     = sampleRate;
	want.format   = AUDIO_F32;
	want.channels = 2; // stereo
	want.samples  = 1024; // must be a power of 2
	want.callback = audioCallback;
	AudioDevice = SDL_OpenAudioDevice(
		NULL, 0, &want, &audioSpec, SDL_AUDIO_ALLOW_FORMAT_CHANGE
	);_sdlec
	#ifdef LOG_AUDIOSPEC_TO
	fprintf(LOG_AUDIOSPEC_TO, "want:\n");
	logSpec(want);
	fprintf(LOG_AUDIOSPEC_TO, "audioSpec:\n");
	logSpec(audioSpec);
	#endif
	if (audioSpec.format != want.format) {
		printf("\nCouldn't get Float32 audio format.\n");
		return __LINE__;
	}
	sampleRate = audioSpec.freq;
	floatStreamSize = audioSpec.size/sizeof(float);
	voiceCount = initVoiceCount;
	voices = calloc(sizeof(voice)*voiceCount);
	return 0;
}
int closeVoices(void) {
	SDL_CloseAudioDevice(AudioDevice);_sdlec
	free(voices);
	return 0;
}
void unpauseAudio(void) {SDL_PauseAudioDevice(AudioDevice, 0);_sdlec}
void   pauseAudio(void) {SDL_PauseAudioDevice(AudioDevice, 1);_sdlec}
