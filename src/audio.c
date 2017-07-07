// "frequency" or "freq" refers to a Hz value, and "pitch" refers to
// a numeric musical note with 0 representing C0, 12 for C1, etc..
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <SDL2/SDL.h>

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
double practicallySilent = 0.001; // voices with lower volume will be ignored

SDL_AudioDeviceID AudioDevice;
SDL_AudioSpec audioSpec;


//void buildSineWave(float *data, uint32_t length) {
//	fr (i, length) data[i] = sin(i*(tau/length));
//}

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

void audioCallback(void *unused, uint8_t *byteStream, int byteStreamLength) {
	static float rampL = 0.0;
	static float rampR = 0.0;
	const float rampLIncrem = freqFromPitch(45)/sampleRate; // A3
	const float rampRIncrem = freqFromPitch(49)/sampleRate; // C#4
	float *floatStream = (float*)byteStream;
	for (int i = 0; i < floatStreamSize; i += 2) {
		rampL += rampLIncrem;
		rampR += rampRIncrem;
		while (rampL > 1.0) rampL -= 1.0;
		while (rampR > 1.0) rampR -= 1.0;
		floatStream[i  ] = rampL;
		floatStream[i+1] = rampR;
	}
}

int initAudio(void) {
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
	SDL_PauseAudioDevice(AudioDevice, 0);_sdlec // unpause audio.
	return 0;
}
int closeAudio(void) {
	SDL_CloseAudioDevice(AudioDevice);_sdlec
	return 0;
}
