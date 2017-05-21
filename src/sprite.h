#pragma once

#include <stdint.h>
#include "optionsAndErrors.h"

typedef struct {
	float    dstCX; // destination center X position
	float    dstCY; // destination center Y position
	float    dstHW; // destination half width,  set negative to mirror x
	float    dstHH; // destination half height, set negative to mirror y
	uint16_t srcX;  // source top left corner
	uint16_t srcY;  // source top left corner
	uint16_t srcW;  // source width
	uint16_t srcH;  // source height
	uint16_t mulR;  // normalized
	uint16_t mulG;  // normalized
	uint16_t mulB;  // normalized
	uint16_t mulO;  // normalized
} sprite;

#ifdef LOG_VERTEX_DATA_TO
void printSprites(sprite *sprites, int count, int line);
#endif
