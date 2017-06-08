
#include <stdlib.h>

#include "initSprites.h"
#include "fileTools.h"
#include "cleanTxtFile.h"
#include "misc.h"
#include "../img/texAtlas.h"


#ifdef LOG_VERTEX_DATA_TO
void printSprites(sprite *sprites, int count, int line) {
	if (!sprites) {
		fprintf(LOG_VERTEX_DATA_TO,
			"ERROR: sprites pointer given at line %i is NULL\n",
			line
		);
		return;
	}
	fprintf(LOG_VERTEX_DATA_TO, "SPRITE DATA, %i count, line %i\n", count, line);
	fr (i, count) {
		fprintf(LOG_VERTEX_DATA_TO,
			"# %i\n"
			"\tdstCX:%9.2f\n"
			"\tdstCY:%9.2f\n"
			"\tdstHW:%9.2f\n"
			"\tdstHH:%9.2f\n"
			"\tsrcX:%6i\n"
			"\tsrcY:%6i\n"
			"\tsrcW:%6i\n"
			"\tsrcH:%6i\n"
			"\tmulR:%6i\n"
			"\tmulG:%6i\n"
			"\tmulB:%6i\n"
			"\tmulO:%6i\n",
			i,
			sprites[i].dstCX,
			sprites[i].dstCY,
			sprites[i].dstHW,
			sprites[i].dstHH,
			sprites[i].srcX,
			sprites[i].srcY,
			sprites[i].srcW,
			sprites[i].srcH,
			sprites[i].mulR,
			sprites[i].mulG,
			sprites[i].mulB,
			sprites[i].mulO
		);
	}
}
#endif

char *const txtPath = "testFile.txt";
const int   railLength = 8; // character widths between gun and queue
int         visCharBeg;
int         charsSize;
char       *chars = NULL;
sprite     *charSprites = NULL;
int         charCount;
int         visCharEnd;
void initCharSprites(void) {
	const int fileCharCount = getFileSize(txtPath);
	visCharBeg  = railLength;
	charsSize   = visCharBeg + fileCharCount;
	chars       = malloc(sizeof(char)*charsSize);   // free in "frameLoop.c"
	charSprites = malloc(sizeof(sprite)*charsSize); // free in "frameLoop.c"
	charCount   = cleanTxtFile(txtPath, &chars[visCharBeg], fileCharCount);
	visCharEnd  = visCharBeg + charCount;
	for (int cPos = visCharBeg, row = 0, col = 0; cPos < visCharEnd; cPos++) {
		charSprites[cPos].dstCX = 0.0 + col*texAtlGlyphW;
		charSprites[cPos].dstCY = 0.0 - row*texAtlGlyphH;
		charSprites[cPos].dstHW = texAtlGlyphW/2.0;
		charSprites[cPos].dstHH = texAtlGlyphH/2.0;
		charSprites[cPos].srcX  = texAtlGlyphPosX(chars[cPos]);
		charSprites[cPos].srcY  = texAtlGlyphPosY(chars[cPos]);
		charSprites[cPos].srcW  = texAtlGlyphW;
		charSprites[cPos].srcH  = texAtlGlyphH;
		charSprites[cPos].mulR  = UINT16_MAX;
		charSprites[cPos].mulG  = UINT16_MAX;
		charSprites[cPos].mulB  = UINT16_MAX;
		charSprites[cPos].mulO  = UINT16_MAX;
		col++;
		if (chars[cPos] == delim) {
			row++;
			col = 0;
		}
	}
	#ifdef LOG_VERTEX_DATA_TO
	fprintf(LOG_VERTEX_DATA_TO, "\nWORD QUEUE\n");
	printSprites(&charSprites[visCharBeg], charCount, __LINE__);
	#endif
}

const int beamCharPerWidth = 2; // affects density of beam
int       beamSpritesSize;
sprite   *beamSprites = NULL;
void initBeamSprites(void) {
	beamSpritesSize = (railLength+maxWordSize) * beamCharPerWidth;
	beamSprites = malloc(sizeof(sprite)*beamSpritesSize); // free in "frameLoop.c"
}

float       spiroExploSpeed = 1.0/240.0;
const int   spiroSpritesSize = 2048; // a guess, raise it if you hit it
sprite     *spiroSprites;
spirograph *visSpiros;
const int   glyphSpirosSize = texAtlGlyphsCount;
spirograph *glyphSpiros;
void initSpiros(void) {
	spiroSprites = malloc(sizeof(sprite)*spiroSpritesSize); // free in "cleanup.c"
	visSpiros = malloc(sizeof(spirograph)*visSpirosSize); // free in "cleanup.c"
	glyphSpiros = malloc(sizeof(spirograph)*glyphSpirosSize); // free in "cleanup.c"
	fr (i, visSpirosSize) visSpiros[i].exploPhase = -1.0;
	fr (i, glyphSpirosSize) {
		fr (arm, spiroArmCount) {
			glyphSpiros[i].arms[arm].armLength = (spiroArmCount-arm) * 10 + 1;
			glyphSpiros[i].arms[arm].posX = 0.0;
			glyphSpiros[i].arms[arm].posY = 0.0;
			glyphSpiros[i].arms[arm].revsWithinFrame = arm%2 ? arm*3.0 : arm*-3.0;
			glyphSpiros[i].arms[arm].glyphRevsWithinFrame = 1.0;
		}
		fr (arm, spiroArmCount) glyphSpiros[i].offsets[arm] = 0.0;
		fr (arm, spiroArmCount) glyphSpiros[i].offsetVelocs[arm] = arm*0.003;
		glyphSpiros[i].exploPhase = -1.0;
		glyphSpiros[i].stampEnablePerArm = 0xfffc;
		glyphSpiros[i].ticksPerFrame = 160;
	}
}


void initSprites(void) {
	initCharSprites();
	initBeamSprites();
	initSpiros();
}
