

#include <stdint.h>
#include "optionsAndErrors.h"

typedef struct {
	float    dstCX; // destination center X position
	float    dstCY; // destination center Y position
	// 8 bytes
	float    dstHW; // destination half width,  set negative to mirror x
	float    dstHH; // destination half height, set negative to mirror y
	// 16 bytes
	uint16_t srcX;  // source top left corner
	uint16_t srcY;  // source top left corner
	// 20 bytes
	uint16_t srcW;  // source width
	uint16_t srcH;  // source height
	// 24 bytes
	uint8_t  mulR;  // normalized
	uint8_t  mulG;  // normalized
	uint8_t  mulB;  // normalized
	uint8_t  mulO;  // normalized
	// 28 bytes
	float    rot;   // 0 to 1 => 1 whole clockwise rotation
	// 32 bytes
} sprite;

#ifdef LOG_VERTEX_DATA_TO
void printSprites(sprite *sprites, int count, int line);
#endif

typedef struct {
	float armLength;            // radius
	float posX;                 // changes with each tick
	float posY;                 // changes with each tick
	float revsWithinFrame;      // rotate arm   this/ticksPerFrame each tick
	float glyphRevsWithinFrame; // rotate glyph this/ticksPerFrame each tick
} spiroArm;

#define spiroArmCount 4
typedef struct {
	spiroArm arms[spiroArmCount];
	float    offsets[spiroArmCount]; // moves between frames
	float    offsetVelocs[spiroArmCount]; // offset moves by this much
	float    exploPhase; // phase of "explosion"
	uint16_t stampEnablePerArm; // 1 bit per arm
	uint16_t ticksPerFrame;
} spirograph;


double hueFromChar(char c);
void setColorFromPhase(sprite *s, double phase, double hue);


extern int beamVertBeg;
extern int gunVertBeg;
extern int menuCursorVertBeg;
extern int charVertBeg;
extern int spiroVertBeg;
extern int vertBufSize;


extern const int gunDistance;
extern int       beamSize;
extern sprite   *beamSprites;
extern const int gunSpritesSize;
extern sprite   *gunSprites;
extern const int menuCursorSpritesSize;
extern sprite   *menuCursorSprites;

extern int       charsSize;
extern char     *chars;
extern sprite   *charSprites;
extern int       fileNamesCharCount;
extern int       fileNamesSize;
extern char     *fileNames;
extern int       courseCount;


extern double      spiroExploSpeed;
extern const int   spiroSpritesSize;
extern sprite     *spiroSprites;
#define            visSpirosSize 8
extern spirograph *visSpiros;
extern const int   glyphSpirosSize;
extern spirograph *glyphSpiros;

#define txtOriginX_ 0
#define txtOriginY_ 0


void initSprites(void);
void initMainMenuSprites(void);
int  initWordQueueSprites(int courseIndex);


enum {quitGame, mainMenu, glyphReactor, spiroViewer};
extern int whereAreWe;
