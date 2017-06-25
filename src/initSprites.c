
#include <stdlib.h>
#include <dirent.h>

#include "initSprites.h"
#include "initBounceEnv.h"
#include "initOpenGl.h"
#include "fileTools.h"
#include "cleanTxtFile.h"
#include "initWindow.h"
#include "fromHue.h"
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
			sprites[i].mulO,
			sprites[i].rot
		);
	}
}
#endif

double hueFromChar(char c) {
	return fractionalPart(7.0*(double)(c-texAtlGlyphsAsciiStart)/texAtlGlyphsCount);
}
void setColorFromPhase(sprite *s, double phase, double hue) {
	const double diff  = pow(phase, 0.3);
	const double start = 1.0 - diff;
	s->mulR  = 0xff * (start + diff*redFromHue(hue));
	s->mulG  = 0xff * (start + diff*grnFromHue(hue));
	s->mulB  = 0xff * (start + diff*bluFromHue(hue));
	s->mulO  = 0xff;
}


int beamVertBeg = 0;
int charVertBeg;
int gunVertBeg;
int spiroVertBeg;
int vertBufSize;

const int gunDistance = 12; // character widths between gun and queue
int       beamSize;
sprite   *beamSprites = NULL;
const int gunSpritesSize = 1;
sprite   *gunSprites;

#define coursesFolderName "courses"
int         visCharBeg;
char       *chars = NULL;
sprite     *charSprites = NULL;
int         charsSize;
int         charCount;
int         visCharEnd;
char       *fileNames = NULL;
int         fileNamesSize = 0;
const char  fileNameDelim = '\n';
const int   maxFileSize = 0x7fff;
int whereAreWe = mainMenu;
void init(void) {
	// init sprites
	beamSize = gunDistance*4;
	beamSprites = malloc(sizeof(sprite)*beamSize); // free in "frameLoop.c"
	// init gun sprites
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
	// load courses
	DIR *dir;
	struct dirent *ent;
	int largestFileSize = 0;
	// path is relative to where you run the program
	if ((dir = opendir(coursesFolderName)) != NULL) {
		// get file sizes
		for (int i = 0; (ent = readdir(dir)) != NULL; i++) {
			if (ent->d_name[0] == '.') continue;
			int fSize = getFileSize(ent->d_name);
			if (fSize > largestFileSize) largestFileSize = fSize;
			for (int j = 0; ent->d_name[j]; fileNamesSize++, j++);
			fileNamesSize++; // one more for delim
		}
		charsSize = largestFileSize > maxFileSize ? maxFileSize : largestFileSize;
		fileNames = malloc(sizeof(char)*fileNamesSize); // free in "frameLoop.c"
		chars = malloc(sizeof(char)*charsSize); // free in "frameLoop.c"
		charSprites = malloc(sizeof(sprite)*charsSize); // free in "frameLoop.c"
		// store file names
		dir = opendir(coursesFolderName);
		int fni = 0;
		for (int i = 0; (ent = readdir(dir)) != NULL; i++) {
			if (ent->d_name[0] == '.') continue;
			for (int j = 0;; j++, fni++) {
				if (!ent->d_name[j]) {
					fileNames[fni] = fileNameDelim;
					fni++;
					break;
				}
				fileNames[fni] = ent->d_name[j];
			}
		}
		fileNames[fni] = '\0';
		closedir(dir);
	}
	// init opengl
	gunVertBeg   = beamSize;
	charVertBeg  = gunVertBeg + gunSpritesSize;
	spiroVertBeg = charVertBeg + charsSize;
	vertBufSize  = charsSize + beamSize + spiroSpritesSize;
	initOpenGl();
	// upload gun sprites
	glBufferSubData(
		GL_ARRAY_BUFFER,             // GLenum        target
		gunVertBeg*sizeof(sprite),   // GLintptr      offset
		sizeof(sprite),              // GLsizeiptr    size
		(const GLvoid*)gunSprites    // const GLvoid *data
	);
}


void initMainMenuSprites(void) {
	visCharBeg = beamSize;
	charCount = fileNamesSize;
	visCharEnd = visCharBeg + fileNamesSize;
	
	printf("fileNames: \n%s\n", fileNames);
	printf("fileNamesSize: %i\n", fileNamesSize);
	
	
	for (int cPos = visCharBeg, fncPos = 0, row = 0, col = 0;;) {
		if (fileNames[fncPos] == fileNameDelim) {
			row++;
			col = 0;
			fncPos++;
			cPos++;
			continue;
		}
		if (!fileNames[fncPos]) {
			visCharEnd = cPos;
			break;
		}
		if (fncPos >= fileNamesSize) _SHOULD_NOT_BE_HERE_;
		
		printf("fncPos: %2i, fileNames[fncPos]: %c\n", fncPos, fileNames[fncPos]);
		
		charSprites[cPos].dstCX = 0.0 + col*texAtlGlyphW;
		charSprites[cPos].dstCY = 0.0 - row*texAtlGlyphH;
		charSprites[cPos].dstHW = texAtlGlyphW/2.0;
		charSprites[cPos].dstHH = texAtlGlyphH/2.0;
		charSprites[cPos].srcX  = texAtlGlyphPosX(fileNames[fncPos]);
		charSprites[cPos].srcY  = texAtlGlyphPosY(fileNames[fncPos]);
		charSprites[cPos].srcW  = texAtlGlyphW;
		charSprites[cPos].srcH  = texAtlGlyphH;
		charSprites[cPos].mulR  = 0xff;
		charSprites[cPos].mulG  = 0xff;
		charSprites[cPos].mulB  = 0xff;
		charSprites[cPos].mulO  = 0xff;
		charSprites[cPos].rot   = 0.0;
		col++;
		fncPos++;
		cPos++;
	}
	glBufferSubData(
		GL_ARRAY_BUFFER,                        // GLenum        target
		visCharVertBeg_*sizeof(sprite),         // GLintptr      offset
		visCharCount_*sizeof(sprite),           // GLsizeiptr    size
		(const GLvoid*)&charSprites[visCharBeg] // const GLvoid *data
	);
	#ifdef LOG_VERTEX_DATA_TO
	fprintf(LOG_VERTEX_DATA_TO, "\nCOURSE LIST\n");
	printSprites(&charSprites[visCharBeg], charCount, __LINE__);
	#endif
}
void initWordQueueSprites(const char* path) {
	const int fileCharCount = getFileSize(path);
	
	printf("fileCharCount: %i\n", fileCharCount);
	
	charCount = cleanTxtFile(path, &chars[visCharBeg], fileCharCount);
	visCharEnd = visCharBeg + charCount;
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
	glBufferSubData(
		GL_ARRAY_BUFFER,                        // GLenum        target
		visCharVertBeg_*sizeof(sprite),         // GLintptr      offset
		visCharCount_*sizeof(sprite),           // GLsizeiptr    size
		(const GLvoid*)&charSprites[visCharBeg] // const GLvoid *data
	);
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
		glyphSpiros[i].arms[0].armLength = 1.2*videoH;
		glyphSpiros[i].arms[0].revsWithinFrame = 1.0;
		
		glyphSpiros[i].arms[1].armLength = 0.2*videoH;
		glyphSpiros[i].arms[1].revsWithinFrame = (1 + i%2)*2 + 1;
		
		glyphSpiros[i].arms[2].armLength = 0.1*videoH;
		glyphSpiros[i].arms[2].revsWithinFrame = (1 + i%4)*4 + 1;
		
		glyphSpiros[i].arms[3].armLength = 0.1*videoH;
		glyphSpiros[i].arms[3].revsWithinFrame = (1 + i%4)*2 + 1;
		glyphSpiros[i].arms[3].glyphRevsWithinFrame = 4.0;
		
		fr (arm, spiroArmCount) glyphSpiros[i].offsets[arm] = 0.0;
		fr (arm, spiroArmCount) glyphSpiros[i].offsetVelocs[arm] = arm*0.005*(1 - 2*(arm%2));
		glyphSpiros[i].exploPhase = 1.0;
		glyphSpiros[i].stampEnablePerArm = 0x0008;
		glyphSpiros[i].ticksPerFrame = 400;
	}
}


void initSprites(void) {
	init();
	initMainMenuSprites();
	initSpiros();
	initBounceEnv();
}
