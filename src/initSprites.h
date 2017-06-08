

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

typedef struct {
	float armLength;            // radius
	float rotBetweenFrames;     // offset that results in animation
	float revsWithinFrame;      // rotate arm   this/ticksPerFrame each tick
	float glyphRevsWithinFrame; // rotate glyph this/ticksPerFrame each tick
} spiroArm;

#define spiroArmCount 6
typedef struct {
	spiroArm arms[spiroArmCount];
	float    positions[spiroArmCount]; // moves between frames
	float    exploPhase; // phase of "explosion"
	uint16_t stampEnablePerArm; // 1 bit per arm
	uint16_t ticksPerFrame;
} spirograph;



extern char *const txtPath;
extern const int   railLength;
extern int         visCharBeg;
extern int         charsSize;
extern char       *chars;
extern sprite     *charSprites;
//extern int         charCount;
extern int         visCharEnd;
#define visCharCount_ (visCharEnd-visCharBeg)

extern const int   beamCharPerWidth;
extern int         beamSpritesSize;
extern sprite     *beamSprites;

extern float       spiroExploSpeed;
extern const int   spiroSpritesSize;
extern sprite     *spiroSprites;
#define            visSpirosSize 8
extern spirograph *visSpiros;
extern const int   glyphSpirosSize;
extern spirograph *glyphSpiros;

#define txtOriginX_ (-railLength*texAtlGlyphW - 64)
#define txtOriginY_ (videoHH_/4)

void initSprites(void);
