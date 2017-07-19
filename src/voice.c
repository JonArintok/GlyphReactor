// "frequency" or "freq" refers to a Hz value, and "pitch" refers to
// a numeric musical note with 0 representing C0, 12 for C1, etc..
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <SDL2/SDL.h>

#include "voice.h"
#include "misc.h"
#include "optionsAndErrors.h"

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

void loopOsc(osc *o) {
	const double p = o->pos;
	if      (p > 1) o->pos -= (long)p;
	else if (p < 0) o->pos -= (long)p-1;
}
void clampOsc(osc *o) {
	const double p = o->pos;
	if (p > 1) {
		o->pos = 1;
		o->spd = 0;
	}
	else if (p < 0) {
		o->pos = 0;
		o->spd = 0;
	}
}

float readOsc(const osc o) {
	return o.shape[(long)(o.pos * (o.shapeSize-1))];
}

void audioCallback(void *unused, uint8_t *byteStream, int byteStreamLength) {
	float *floatStream = (float*)byteStream;
	float enabledVoiceCount = 0;
	fr (v, voiceCount) {
		if (!voices[v].enable) continue;
		enabledVoiceCount++;
		const double spdModIncrem = voices[v].spdMod.spd / sampleRate;
		const double ampModIncrem = voices[v].ampMod.spd / sampleRate;
		const double rightFactor = (voices[v].pan+1.0)/2.0;
		const double leftFactor  = 1.0 - rightFactor;
		for (int s = 0; s < floatStreamSize; s += 2) {
			voices[v].spdEnv.pos += voices[v].spdEnv.spd / sampleRate;
			clampOsc(&voices[v].spdEnv);
			voices[v].spdMod.pos += spdModIncrem;
			loopOsc(&voices[v].spdMod);
			voices[v].wave.pos += (voices[v].wave.spd * readOsc(voices[v].spdEnv) * readOsc(voices[v].spdMod)) / sampleRate;
			loopOsc(&voices[v].wave);
			voices[v].ampEnv.pos += voices[v].ampEnv.spd / sampleRate;
			clampOsc(&voices[v].ampEnv);
			voices[v].ampMod.pos += ampModIncrem;
			loopOsc(&voices[v].ampMod);
			const double sample = readOsc(voices[v].wave) * readOsc(voices[v].ampMod) * readOsc(voices[v].ampEnv);
			floatStream[s  ] += sample * leftFactor;
			floatStream[s+1] += sample * rightFactor;
		}
	}
	fr (s, floatStreamSize) floatStream[s] /= enabledVoiceCount;
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
	voices = calloc(voiceCount, sizeof(voice));
	return 0;
}
int closeVoices(void) {
	SDL_CloseAudioDevice(AudioDevice);_sdlec
	free(voices);
	return 0;
}
void unpauseAudio(void) {SDL_PauseAudioDevice(AudioDevice, 0);_sdlec}
void   pauseAudio(void) {SDL_PauseAudioDevice(AudioDevice, 1);_sdlec}
