
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
	fprintf(LOG_VERTEX_DATA_TO, "SPRITE DATA, %i count\n", count);
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
const int   beamCharPerWidth = 2; // affects kerning of beam glyphs
int         beamSpritesSize;
sprite     *beamSprites = NULL;
void initSprites(void) {
	// reactor character data
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
	// beam data
	beamSpritesSize = (railLength+maxWordSize) * beamCharPerWidth;
	beamSprites = malloc(sizeof(sprite)*beamSpritesSize); // free in "frameLoop.c"
}
