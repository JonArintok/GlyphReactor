
typedef struct {
	float    *shape;     // values are expected to be between -1 and 1
	uint64_t  shapeSize; // total count of samples(floats) in shape
	float     pos;       // position in shape
	float     speed;     // 1.0 results in 1 shape sample per output sample
	float     amp;       // multiply shape output sample
	uint16_t  interp;    // enumerated, how to interpolate between shape samples
	uint16_t  trip;      // enumerated, what to do when pos goes out of bounds
} envel;
enum {interp_nearest, interp_linear};
enum {
	trip_once,  // envelope is disabled once reaching the end of shape
	trip_clamp, // envelope will continue outputing the last sample of shape
	trip_loop,  // envelope will loop
	trip_bounce // speed will be negated at the boundaries
};

typedef struct {
	envel wave;      // mono audio clip or waveform
	envel ampMod;    // modulates volume of wave (multiplied)
	envel speedMod;  // modulates speed of wave (multiplied)
	float pan;       // -1.0 is all the way left, 1.0 is all the way right
} voice;



int initAudio(void);
int closeAudio(void);
