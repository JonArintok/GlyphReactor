
typedef struct {
	float    *shape;     // values are expected to be between -1 and 1
	uint32_t  shapeSize; // total count of samples(floats) in shape
	float     amp;       // multiply shape output sample
	double    pos;       // position in shape, 0 to 1
	double    spd;       // 1.0 results in 1 shape sample per output sample
} osc;
typedef struct {
	osc     wave;   // looped
	osc     ampMod; // looped, multiply wave amp
	osc     spdMod; // looped, multiply wave speed
	osc     ampEnv; // clamped, multiply wave amp
	osc     spdEnv; // clamped, multiply wave speed
	float   pan;    // -1.0 is all the way left, 1.0 is all the way right
	uint8_t enable; // voice will be ignored if this is false
} voice;

int initVoices(int initVoiceCount);
int closeVoices(void);
void unpauseAudio(void);
void pauseAudio(void);
