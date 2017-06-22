
#include <stdlib.h>

#include "initSprites.h"
#include "fileTools.h"
#include "cleanTxtFile.h"
#include "initWindow.h"
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
			"\tmulO:%6i\n"
			"\trot:%9.2f\n",
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
			sprites[i].rot
		);
	}
}
#endif

const int gunDistance = 12; // character widths between gun and queue
int       beamSize;
sprite   *beamSprites = NULL;
const int gunSpritesSize = 1;
sprite   *gunSprites;
void initBeamSprites(void) {
	beamSize = gunDistance*4;
	beamSprites = malloc(sizeof(sprite)*beamSize); // free in "frameLoop.c"
	gunSprites = malloc(sizeof(sprite)*gunSpritesSize); // free in "frameLoop.c"
	gunSprites[0].dstCX = -gunDistance*texAtlGlyphW - texAtlGunW/2;
	gunSprites[0].dstCY = txtOriginY_ - texAtlGunYoffset;
	gunSprites[0].dstHW = texAtlGunW/2.0;
	gunSprites[0].dstHH = texAtlGunH/2.0;
	gunSprites[0].srcX  = texAtlGunX;
	gunSprites[0].srcY  = texAtlGunY;
	gunSprites[0].srcW  = texAtlGunW;
	gunSprites[0].srcH  = texAtlGunH;
	gunSprites[0].mulR  = 0xff;
	gunSprites[0].mulG  = 0xff;
	gunSprites[0].mulB  = 0xff;
	gunSprites[0].mulO  = 0xff;
	gunSprites[0].rot   = 0.0;
}

char *const txtPath = "testFile.txt";
int         visCharBeg;
int         charsSize;
char       *chars = NULL;
sprite     *charSprites = NULL;
int         charCount;
int         visCharEnd;
void initCharSprites(void) {
	const int fileCharCount = getFileSize(txtPath);
	visCharBeg  = beamSize;
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
		charSprites[cPos].mulR  = 0xff;
		charSprites[cPos].mulG  = 0xff;
		charSprites[cPos].mulB  = 0xff;
		charSprites[cPos].mulO  = 0xff;
		charSprites[cPos].rot   = 0.0;
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

double      spiroExploSpeed = 0.003;
const int   spiroSpritesSize = 4096; // a guess, raise it if you hit it
sprite     *spiroSprites;
spirograph *visSpiros;
const int   glyphSpirosSize = texAtlGlyphsCount;
spirograph *glyphSpiros;
void initSpiros(void) {
	spiroSprites = malloc(sizeof(sprite)*spiroSpritesSize); // free in "cleanup.c"
	visSpiros = malloc(sizeof(spirograph)*visSpirosSize); // free in "cleanup.c"
	glyphSpiros = malloc(sizeof(spirograph)*glyphSpirosSize); // free in "cleanup.c"
	fr (i, visSpirosSize) visSpiros[i].exploPhase = 1.0;
	fr (i, glyphSpirosSize) {
		fr (arm, spiroArmCount) {
			glyphSpiros[i].arms[arm].armLength = 0.0;
			glyphSpiros[i].arms[arm].posX = 0.0;
			glyphSpiros[i].arms[arm].posY = 0.0;
		}
		glyphSpiros[i].arms[0].armLength = 1.6*videoH;
		glyphSpiros[i].arms[0].revsWithinFrame = 1.0;
		
		glyphSpiros[i].arms[1].armLength = 0.2*videoH;
		glyphSpiros[i].arms[1].revsWithinFrame = (1 + i%2)*2 + 1;
		
		glyphSpiros[i].arms[2].armLength = 0.2*videoH;
		glyphSpiros[i].arms[2].revsWithinFrame = (1 + i%4)*2 + 1;
		glyphSpiros[i].arms[2].glyphRevsWithinFrame = 1.0;
		
		fr (arm, spiroArmCount) glyphSpiros[i].offsets[arm] = 0.0;
		fr (arm, spiroArmCount) glyphSpiros[i].offsetVelocs[arm] = arm*0.02;
		glyphSpiros[i].exploPhase = 1.0;
		glyphSpiros[i].stampEnablePerArm = 0x0004;
		glyphSpiros[i].ticksPerFrame = 400;
	}
}


void initSprites(void) {
	initBeamSprites();
	initCharSprites();
	initSpiros();
}
