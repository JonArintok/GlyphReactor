
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
void setHueFromPhase(sprite *s, double phase, double hue) {
	const double diff  = pow(phase, 0.3); // curve to taste
	const double start = 1.0 - diff;
	s->mulR = 0xff * (start + diff*redFromHue(hue));
	s->mulG = 0xff * (start + diff*grnFromHue(hue));
	s->mulB = 0xff * (start + diff*bluFromHue(hue));
	s->mulO = 0xff;
}

const char gameOverMessage[gameOverMessageLength] = "INCOMPLETE";

int beamVertBeg;
int gunVertBeg;
int menuCursorVertBeg;
int charVertBeg;
int messageVertBeg;
int spiroVertBeg;
int vertBufSize;

const int gunDistance = 12; // character widths between gun and queue
int       beamSize;
sprite   *beamSprites = NULL;
const int gunSpritesSize = 2;
sprite   *gunSprites = NULL;
const int messageSpritesSize = gameOverMessageLength;
sprite   *messageSprites = NULL;
const int menuCursorSpritesSize = 3;
sprite   *menuCursorSprites;


const char *coursesFolderName = "courses";
int         charsSize;
char       *chars = NULL;
sprite     *charSprites = NULL;
int         fileNamesCharCount;
int         fileNamesSize;
char       *fileNames = NULL;
const char  fileNameDelim = '\n';
int         courseCount = 0;
const int   maxFileSize = 0x7fff;
int whereAreWe = screen_mainMenu;
void init(void) {
	// init sprites
	beamSize = gunDistance*4;
	beamSprites = malloc(sizeof(sprite)*beamSize);
	// init gun sprites
	const float texAtlGunYoffset = (texAtlGunW-texAtlGunH)/2;
	gunSprites = malloc(sizeof(sprite)*gunSpritesSize);
	// belt tube
	gunSprites[0].dstCX = -gunDistance*texAtlGlyphW - texAtlGunW/2.0;
	gunSprites[0].dstCY = -videoH/4.0;
	gunSprites[0].dstHW = texAtlBeltW/2.0;
	gunSprites[0].dstHH = videoH/4.0;
	gunSprites[0].srcX  = texAtlBeltX;
	gunSprites[0].srcY  = texAtlBeltY;
	gunSprites[0].srcW  = texAtlBeltW;
	gunSprites[0].srcH  = texAtlBeltH;
	gunSprites[0].mulR  = 0xff;
	gunSprites[0].mulG  = 0xff;
	gunSprites[0].mulB  = 0xff;
	gunSprites[0].mulO  = 0xff;
	gunSprites[0].rot   = 0.0;
	// sphere
	gunSprites[1].dstCX = -gunDistance*texAtlGlyphW - texAtlGunW/2;
	gunSprites[1].dstCY = txtOriginY_ + texAtlGunYoffset;
	gunSprites[1].dstHW = texAtlGunW/2.0;
	gunSprites[1].dstHH = texAtlGunH/2.0;
	gunSprites[1].srcX  = texAtlGunX;
	gunSprites[1].srcY  = texAtlGunY;
	gunSprites[1].srcW  = texAtlGunW;
	gunSprites[1].srcH  = texAtlGunH;
	gunSprites[1].mulR  = 0xff;
	gunSprites[1].mulG  = 0xff;
	gunSprites[1].mulB  = 0xff;
	gunSprites[1].mulO  = 0xff;
	gunSprites[1].rot   = 0.0;
	// message
	messageSprites = malloc(sizeof(sprite)*messageSpritesSize);
	// init menuCursorSprites
	{
		const float cursorX = txtOriginX_ - texAtlGlyphW*1.5;
		const float cursorYspacing = 20;
		menuCursorSprites = malloc(sizeof(sprite)*menuCursorSpritesSize);
		fr (i, menuCursorSpritesSize) {
			menuCursorSprites[i].dstCX = cursorX;
			menuCursorSprites[i].dstHW = texAtlGlyphW/2.0;
			menuCursorSprites[i].dstHH = texAtlGlyphH/2.0;
			menuCursorSprites[i].srcW  = texAtlGlyphW;
			menuCursorSprites[i].srcH  = texAtlGlyphH;
			menuCursorSprites[i].mulR  = 0xff;
			menuCursorSprites[i].mulG  = 0xff;
			menuCursorSprites[i].mulB  = 0xff;
			menuCursorSprites[i].mulO  = 0xff;
			menuCursorSprites[i].rot   = 0.0;
		}
		// u
		menuCursorSprites[0].dstCY = txtOriginY_ + cursorYspacing;
		menuCursorSprites[0].srcX  = texAtlGlyphPosX('u');
		menuCursorSprites[0].srcY  = texAtlGlyphPosY('u');
		// arrow
		menuCursorSprites[1].dstCY = txtOriginY_;
		menuCursorSprites[1].dstHW = texAtlMenuCursorW/2.0;
		menuCursorSprites[1].dstHH = texAtlMenuCursorH/2.0;
		menuCursorSprites[1].srcX  = texAtlMenuCursorX;
		menuCursorSprites[1].srcY  = texAtlMenuCursorY;
		menuCursorSprites[1].srcW  = texAtlMenuCursorW;
		menuCursorSprites[1].srcH  = texAtlMenuCursorH;
		// d
		menuCursorSprites[2].dstCY = txtOriginY_ - cursorYspacing;
		menuCursorSprites[2].srcX  = texAtlGlyphPosX('d');
		menuCursorSprites[2].srcY  = texAtlGlyphPosY('d');
	}
	#ifdef LOG_VERTEX_DATA_TO
	fprintf(LOG_VERTEX_DATA_TO, "\nMENU CURSOR\n");
	printSprites(menuCursorSprites, menuCursorSpritesSize, __LINE__);
	#endif
	// load courses
	DIR *dir;
	struct dirent *ent;
	int largestFileSize = 0;
	// path is relative to where you run the program
	if ((dir = opendir(coursesFolderName)) != NULL) {
		// get file sizes
		for (int i = 0; (ent = readdir(dir)) != NULL; i++) {
			if (ent->d_name[0] == '.') continue;
			courseCount++;
			int fSize = getFileSize(ent->d_name);
			if (fSize > largestFileSize) largestFileSize = fSize;
			for (int j = 0; ent->d_name[j]; fileNamesSize++, j++);
			fileNamesSize++; // one more for delim
		}
		if (largestFileSize+gunDistance > maxFileSize) {
			charsSize = largestFileSize+gunDistance;
		} else charsSize = maxFileSize;
		fileNames = malloc(sizeof(char)*fileNamesSize);
		chars = malloc(sizeof(char)*charsSize);
		charSprites = malloc(sizeof(sprite)*charsSize);
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
	beamVertBeg       = 0;
	gunVertBeg        = beamVertBeg       + beamSize;
	messageVertBeg    = gunVertBeg        + gunSpritesSize;
	menuCursorVertBeg = messageVertBeg    + messageSpritesSize;
	charVertBeg       = menuCursorVertBeg + menuCursorSpritesSize;
	spiroVertBeg      = charVertBeg       + charsSize;
	vertBufSize       = spiroVertBeg      + spiroSpritesSize;
	initOpenGl();
	// upload gun sprites
	glBufferSubData(
		GL_ARRAY_BUFFER,               // GLenum        target
		sizeof(sprite)*gunVertBeg,     // GLintptr      offset
		sizeof(sprite)*gunSpritesSize, // GLsizeiptr    size
		(const GLvoid*)gunSprites      // const GLvoid *data
	);
	// upload menu cursor sprites
	glBufferSubData(
		GL_ARRAY_BUFFER,
		sizeof(sprite)*menuCursorVertBeg,
		sizeof(sprite)*menuCursorSpritesSize,
		(const GLvoid*)menuCursorSprites
	);
}


void initMainMenuSprites(void) {
	for (int cPos = 0, fncPos = 0, row = 0, col = 0;;) {
		if (fileNames[fncPos] == fileNameDelim) {
			row++;
			col = 0;
			fncPos++;
			continue;
		}
		if (!fileNames[fncPos]) {
			fileNamesCharCount = cPos;
			break;
		}
		if (fncPos >= fileNamesSize) _SHOULD_NOT_BE_HERE_;
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
		GL_ARRAY_BUFFER,                   // GLenum        target
		sizeof(sprite)*charVertBeg,        // GLintptr      offset
		sizeof(sprite)*fileNamesCharCount, // GLsizeiptr    size
		(const GLvoid*)charSprites         // const GLvoid *data
	);
	#ifdef LOG_VERTEX_DATA_TO
	fprintf(LOG_VERTEX_DATA_TO, "\nCOURSE LIST\n");
	printSprites(charSprites, fileNamesCharCount, __LINE__);
	#endif
}

int initWordQueueSprites(int courseIndex) {
	// load course file
	char path[maxWordSize];
	int i = 0;
	int fni = 0;
	{
		int delimsEncountered = 0;
		for (; delimsEncountered < courseIndex; fni++) {
			if (fileNames[fni] == fileNameDelim) delimsEncountered++;
		}
	}
	for (; coursesFolderName[i]; i++) path[i] = coursesFolderName[i];
	path[i++] = '/';
	for (; fileNames[fni] && fileNames[fni] != fileNameDelim; i++, fni++) {
		path[i] = fileNames[fni];
	}
	path[i] = '\0';
	const int fileCharCount = getFileSize(path);
	int charCount = cleanTxtFile(path, &chars[gunDistance], fileCharCount);
	// init word queue sprites
	for (
		int cPos = gunDistance, row = 0, col = 0;
		cPos < gunDistance+charCount;
		cPos++
	) {
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
		GL_ARRAY_BUFFER,                          // GLenum        target
		sizeof(sprite)*(charVertBeg+gunDistance), // GLintptr      offset
		sizeof(sprite)*charCount,                 // GLsizeiptr    size
		(const GLvoid*)&charSprites[gunDistance]  // const GLvoid *data
	);
	#ifdef LOG_VERTEX_DATA_TO
	fprintf(LOG_VERTEX_DATA_TO, "\nWORD QUEUE\n");
	printSprites(&charSprites[gunDistance], charCount, __LINE__);
	#endif
	return charCount;
}

double      spiroExploSpeed = 0.003;
const int   spiroSpritesSize = 4096; // a guess, raise it if you hit it
sprite     *spiroSprites;
spirograph *visSpiros;
const int   glyphSpirosSize = texAtlGlyphsCount;
spirograph *glyphSpiros;
void initSpiros(void) {
	spiroSprites = malloc(sizeof(sprite)*spiroSpritesSize);
	visSpiros = malloc(sizeof(spirograph)*visSpirosSize);
	glyphSpiros = malloc(sizeof(spirograph)*glyphSpirosSize);
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

void cleanupSprites(void) {
	free(chars);
	free(fileNames);
	free(gunSprites);
	free(menuCursorSprites);
	free(charSprites);
	free(beamSprites);
}
