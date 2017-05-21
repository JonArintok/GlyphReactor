
#include "../img/texAtlas.h"

#define wordDropEnvCount 64
float wordDropEnv[wordDropEnvCount];

void initBounceEnv(void) {
	wordDropEnv[0] = texAtlGlyphH;
	float vel = 0.8, accel = 0.12;
	const float bounce = -0.5;
	for (int i = 1; i < wordDropEnvCount; i++) {
		float height = wordDropEnv[i-1];
		height -= vel;
		if (height < 0) {
			height *= -1;
			vel *= bounce;
		}
		vel += accel;
		//printf("%3i: vel: %9.6f\n", i, vel);
		wordDropEnv[i] = height;
	}
	wordDropEnv[wordDropEnvCount-1] = 0;
}

float bouncePos(double phase) {
	if (phase > 1 || phase < 0) return 0;
	return wordDropEnv[(int)(wordDropEnvCount*phase)];
}
