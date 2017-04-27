
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define  GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "optionsAndErrors.h"
#include "fileTools.h"
#include "oglTools.h"
#include "misc.h"
#include "timestamp.h"
#include "../img/texAtlas.h"


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

const char delim = ' ';
const int  maxWordSize = 80;
int cleanTxtFile(const char *txtPath, char *chars, int writeLimit) {
	int readLimit = stringFromFile(txtPath, chars, writeLimit);
	int readPos = 0;
	int writPos = 0;
	int writePosInWord = 0;
	while (readPos < writeLimit) {
		if (chars[readPos] == '\0' || readPos > readLimit) {
			chars[writPos] = '\0';
			break;
		}
		if (chars[readPos] < '!' || chars[readPos] > '~') {
			readPos++;
			if (chars[writPos-1] != delim) {
				chars[writPos] = delim;
				writPos++;
				writePosInWord = 0;
			}
			continue;
		}
		chars[writPos] = chars[readPos];
		readPos++;
		writPos++;
		writePosInWord++;
		if (writePosInWord >= maxWordSize) {
			// this overwrites a character, but it's just to prevent memory errors
			chars[writPos] = delim;
			writPos++;
			writePosInWord = 0;
		}
	}
	return writPos;
}



int main(int argc, char **argv) {
	// init SDL and get an opengl window
	int videoW = 1280;
  int videoH =  720;
	#define videoHW_ (videoW/2)
  #define videoHH_ (videoH/2)
	#define scaleX_ (1.0/videoHW_)
	#define scaleY_ (1.0/videoHH_)
	SDL_Window    *window    = NULL;
	SDL_GLContext  GLcontext = NULL;
	SDL_Init(SDL_INIT_VIDEO);_sdlec
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	window = SDL_CreateWindow(
		"GlyphReactor",            //const char* title,
		SDL_WINDOWPOS_UNDEFINED,   //int         x,
		SDL_WINDOWPOS_UNDEFINED,   //int         y,
		videoW,                    //int         w,
		videoH,                    //int         h,
		SDL_WINDOW_OPENGL |        //Uint32      flags
		SDL_WINDOW_RESIZABLE
	);_sdlec
	GLcontext = SDL_GL_CreateContext(window);_sdlec
  SDL_GL_SetSwapInterval(enableVSync);_sdlec
  glewExperimental = GL_TRUE;
  {
  	GLenum r = glewInit();//_glec
		// flush persistent GL_INVALID_ENUM error...
		if (glGetError() == GL_INVALID_ENUM) {}_glec
    if (r != GLEW_OK) {
      printf("GLEW error: %s\n", glewGetErrorString(r));
      return 1;
    }
  }
	#ifdef LOG_GL_ERRORS_TO
  fprintf(LOG_GL_ERRORS_TO,
		"OpenGL version: %s\n",
		glGetString(GL_VERSION)
	);_glec
	glEnable(GL_DEBUG_OUTPUT);_glec
	glDebugMessageCallback(glErrorCallback, NULL);_glec
	#endif
	
	// init character array
	const char *txtPath = "testFile.txt";
	const int railLength = 8;       // character widths between gun and queue
	const int beamCharPerWidth = 2; // affects kerning of beam glyphs
	const int fileCharCount = getFileSize(txtPath);
	int visCharBeg = (railLength+maxWordSize) * beamCharPerWidth;
	int charsSize = visCharBeg + fileCharCount;
	char *chars = malloc(sizeof(char)*charsSize);
	const int charCount = cleanTxtFile(
		txtPath,
		&chars[visCharBeg],
		fileCharCount
	);
	int visCharEnd = visCharBeg + charCount;
	#define visCharCount_ (visCharEnd-visCharBeg)
	
	// init sprite data
	sprite *sprites = malloc(sizeof(sprite)*charsSize);
	#define txtOriginX_ (-railLength*texAtlGlyphW - 64)
	#define txtOriginY_ (videoHH_/4)
	for (int cPos = visCharBeg, row = 0, col = 0; cPos < visCharEnd; cPos++) {
		sprites[cPos].dstCX = 0.0 + col*texAtlGlyphW;
		sprites[cPos].dstCY = 0.0 - row*texAtlGlyphH;
		sprites[cPos].dstHW = texAtlGlyphW/2.0;
		sprites[cPos].dstHH = texAtlGlyphH/2.0;
		sprites[cPos].srcX  = texAtlGlyphPosX(chars[cPos]);
		sprites[cPos].srcY  = texAtlGlyphPosY(chars[cPos]);
		sprites[cPos].srcW  = texAtlGlyphW;
		sprites[cPos].srcH  = texAtlGlyphH;
		sprites[cPos].mulR  = UINT16_MAX;
		sprites[cPos].mulG  = UINT16_MAX;
		sprites[cPos].mulB  = UINT16_MAX;
		sprites[cPos].mulO  = UINT16_MAX;
		col++;
		if (chars[cPos] == delim) {
			row++;
			col = 0;
		}
	}
	#ifdef LOG_VERTEX_DATA_TO
	printSprites(sprites, charCount, __LINE__);
	#endif
	
	// build bounce envelope
	#define wordDropEnvCount 64
	float wordDropEnv[wordDropEnvCount];
	wordDropEnv[0] = texAtlGlyphH;
	{
		float vel = 0.8, accel = 0.12;
		const float bounce = -0.5;
		for (int i = 1; i < wordDropEnvCount; i++) {
			float height = wordDropEnv[i-1];
			height -= vel;
			if (height < 0) {
				height *= -1;
				vel *= bounce;
			}
			vel += accel;
			//printf("%3i: vel: %9.6f\n", i, vel);
			wordDropEnv[i] = height;
		}
	}
	wordDropEnv[wordDropEnvCount-1] = 0;
	
	// init opengl state
	// vertex array object
  GLuint vao;
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glGenVertexArrays(1, &vao);_glec
  glBindVertexArray(vao);_glec
	// vertex buffer object
  GLuint vbo;
  glGenBuffers(1, &vbo);_glec
  glBindBuffer(GL_ARRAY_BUFFER, vbo);_glec
  glBufferData(
    GL_ARRAY_BUFFER,
    charsSize*sizeof(sprite),
    sprites,
    GL_DYNAMIC_DRAW
  );_glec
	// shader program
  GLuint shaderProgram = createShaderProgram(
    "src/shadeVert.glsl",
		NULL,
		NULL,
		"src/shadeGeom.glsl",
    "src/shadeFrag.glsl",
    "shaderProgram"
  );
  if (!shaderProgram) return __LINE__;
  glUseProgram(shaderProgram);_glec
	// attributes
  GLint attr_dstPosCntr = glGetAttribLocation(shaderProgram, "dstPosCntr");_glec
	GLint attr_dstHlfSize = glGetAttribLocation(shaderProgram, "dstHlfSize");_glec
	GLint attr_srcPosTpLt = glGetAttribLocation(shaderProgram, "srcPosTpLt");_glec
	GLint attr_srcSize    = glGetAttribLocation(shaderProgram, "srcSize"   );_glec
	GLint attr_mulColor   = glGetAttribLocation(shaderProgram, "mulColor"  );_glec
  glEnableVertexAttribArray(attr_dstPosCntr);_glec
	glEnableVertexAttribArray(attr_dstHlfSize);_glec
	glEnableVertexAttribArray(attr_srcPosTpLt);_glec
	glEnableVertexAttribArray(attr_srcSize   );_glec
  glEnableVertexAttribArray(attr_mulColor  );_glec
	const GLsizei vertAttrStride = sizeof(sprite);
	glVertexAttribPointer(attr_dstPosCntr, 2, GL_FLOAT,          GL_FALSE, vertAttrStride, (const GLvoid*) 0);_glec
	glVertexAttribPointer(attr_dstHlfSize, 2, GL_FLOAT,          GL_FALSE, vertAttrStride, (const GLvoid*) 8);_glec
  glVertexAttribPointer(attr_srcPosTpLt, 2, GL_UNSIGNED_SHORT, GL_FALSE, vertAttrStride, (const GLvoid*)16);_glec
  glVertexAttribPointer(attr_srcSize,    2, GL_UNSIGNED_SHORT, GL_FALSE, vertAttrStride, (const GLvoid*)20);_glec
  glVertexAttribPointer(attr_mulColor,   4, GL_UNSIGNED_SHORT, GL_TRUE,  vertAttrStride, (const GLvoid*)24);_glec
	// uniforms
	GLint unif_texAtlSize   = glGetUniformLocation(shaderProgram, "texAtlSize");_glec
	GLint unif_scale        = glGetUniformLocation(shaderProgram, "scale");_glec
	GLint unif_translate    = glGetUniformLocation(shaderProgram, "translate");_glec
	glUniform2f(unif_texAtlSize, texAtlW, texAtlH);_glec
	glUniform2f(unif_scale, scaleX_, scaleY_);_glec
	// texture
	GLuint texAtl = 0;
	texFromPng(texAtl, texAtlPath, false);
  glUniform1i(glGetUniformLocation(shaderProgram, "texAtl"), 0);_glec
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);_glec
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);_glec
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);_glec
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);_glec
	// other opengl
	const uint32_t clearColor = 0x224455ff; // rgba
	glClearColori(clearColor);
	glClear(GL_COLOR_BUFFER_BIT);
	#if drawWireFrame
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	#endif
	
	// init frame state
	int stuckCharCount = 0;
	int misBkspCount = 0;
	int curWord = 0;
	int whereCurWordStarted = visCharBeg;
	int frameWhenWordDropped = 0;
	const int beamGlowTime = 60; // frames
	int frameWhenCharEntered = -beamGlowTime;
	int lastCharEntered = '\0';
	int curFrame = 0;
  timestamp ts_oldFrameStart={0,0},ts_newFrameStart={0,0},ts_frameDelta={0,0};
	#ifdef LOG_TIMING_TO
  timestamp ts_compTime = {0,0}, ts_now = {0,0};
  #endif
  clock_gettime(CLOCK_MONOTONIC, &ts_newFrameStart);
	
	// frame loop
	SDL_StartTextInput();
	for (bool running = true; running; curFrame++) {
    ts_oldFrameStart = ts_newFrameStart;
    clock_gettime(CLOCK_MONOTONIC, &ts_newFrameStart);
    getTimeDelta(&ts_oldFrameStart, &ts_newFrameStart, &ts_frameDelta);
    #ifdef LOG_TIMING_TO
    fprintf(LOG_TIMING_TO,
      "ts_frameDelta: %1ld s, %9ld ns\n",
      ts_frameDelta.tv_sec, ts_frameDelta.tv_nsec
    );
    #endif
		char charEntered = '\0';
    SDL_Event event;
		// event loop
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT: running = false; break;
				case SDL_WINDOWEVENT:
					switch (event.window.event) {
						case SDL_WINDOWEVENT_RESIZED:
							videoW = event.window.data1;
							videoH = event.window.data2;
							glViewport(0, 0, videoW, videoH);_glec
							glUniform2f(unif_scale, scaleX_, scaleY_);_glec
            break;
					}
					break;
        case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
						case SDLK_BACKSPACE: charEntered = bkspChar; break;
						case SDLK_ESCAPE:    running = false; break;
					}
					break;
				case SDL_TEXTINPUT: {
					charEntered = event.text.text[0];
          break;
				}
        case SDL_KEYUP:
          break;
      }
    }
		if (!running) break;
		// respond to character entered
		if (charEntered) {
			lastCharEntered = charEntered;
			frameWhenCharEntered = curFrame;
			if (charEntered == bkspChar) {
				if (stuckCharCount) {
					stuckCharCount--;
					visCharBeg++;
				}
				else misBkspCount++;
			}
			else if (charEntered == chars[visCharBeg] && !stuckCharCount) {
				visCharBeg++;
				if (charEntered == delim) {
					curWord++;
					whereCurWordStarted = visCharBeg;
					frameWhenWordDropped = curFrame;
				}
			}
			else {
				visCharBeg--;
				stuckCharCount++;
				if (visCharBeg < 0 || visCharBeg >= visCharEnd) {
					running = false;
					break;
				}
				sprites[visCharBeg] = sprites[visCharBeg+1];
				sprites[visCharBeg].dstCX -= texAtlGlyphW;
				if (sprites[visCharBeg].dstCX <=  0.0 - texAtlGlyphW*railLength) {
					running = false;
					break;
				}
				else {
					chars[visCharBeg] = charEntered;
					sprites[visCharBeg].dstHW = -texAtlGlyphW/2.0;
					sprites[visCharBeg].dstHH = -texAtlGlyphH/2.0;
					sprites[visCharBeg].srcX  = texAtlGlyphPosX(charEntered);
					sprites[visCharBeg].srcY  = texAtlGlyphPosY(charEntered);
					sprites[visCharBeg].mulR  = UINT16_MAX;
					sprites[visCharBeg].mulG  = 0;
					sprites[visCharBeg].mulB  = 0;
					sprites[visCharBeg].mulO  = UINT16_MAX;
					glBufferSubData(
						GL_ARRAY_BUFFER,                    // GLenum        target
						visCharBeg*sizeof(sprite),          // GLintptr      offset
						sizeof(sprite),                     // GLsizeiptr    size
						(const GLvoid*)&sprites[visCharBeg] // const GLvoid *data
					);
				}
			}
		}
		// draw beam
		const float beamPhase = (float)(curFrame-frameWhenCharEntered)/beamGlowTime;
		if (beamPhase <= 1) {
			const int beamSize  = (railLength + (visCharBeg-whereCurWordStarted))*beamCharPerWidth;
			const int beamStart = visCharBeg - beamSize;
			fr (i, beamSize) {
				const int sIndex = beamStart + i;
				sprites[sIndex].dstCX = -railLength*texAtlGlyphW + i*texAtlGlyphW/beamCharPerWidth;
				sprites[sIndex].dstCY = 0;
				sprites[sIndex].dstHW = texAtlGlyphW/2.0;
				sprites[sIndex].dstHH = texAtlGlyphH/2.0;
				sprites[sIndex].srcX  = texAtlGlyphPosX(lastCharEntered);
				sprites[sIndex].srcY  = texAtlGlyphPosY(lastCharEntered);
				sprites[sIndex].srcW  = texAtlGlyphW;
				sprites[sIndex].srcH  = texAtlGlyphH;
				sprites[sIndex].mulR  = 0;
				sprites[sIndex].mulG  = UINT16_MAX;
				sprites[sIndex].mulB  = UINT16_MAX;
				sprites[sIndex].mulO  = UINT16_MAX * (1.0 - beamPhase);
			}
			glBufferSubData(
				GL_ARRAY_BUFFER,                   // GLenum        target
				beamStart*sizeof(sprite),          // GLintptr      offset
				beamSize*sizeof(sprite),           // GLsizeiptr    size
				(const GLvoid*)&sprites[beamStart] // const GLvoid *data
			);_glec
			glUniform2f(unif_translate, txtOriginX_, txtOriginY_);_glec
			glDrawArrays(GL_POINTS, beamStart, beamSize);_glec
		}
		// draw word queue
		if (wordDropEnvCount > curFrame-frameWhenWordDropped) {
			glUniform2f(
				unif_translate,
				txtOriginX_,
				txtOriginY_ + curWord*texAtlGlyphH - wordDropEnv[curFrame-frameWhenWordDropped]
			);_glec
		}
		else {
			glUniform2f(
				unif_translate,
				txtOriginX_,
				txtOriginY_ + curWord*texAtlGlyphH
			);
		}
    glDrawArrays(GL_POINTS, visCharBeg, visCharCount_);_glec
		// finish frame
    #ifdef LOG_TIMING_TO
		clock_gettime(CLOCK_MONOTONIC, &ts_now);
    getTimeDelta(&ts_newFrameStart, &ts_now, &ts_compTime);
    fprintf(LOG_TIMING_TO,
			"ts_compTime: %3ld s, %9ld ns\n",
      ts_compTime.tv_sec, ts_compTime.tv_nsec
    );
    #endif
		SDL_GL_SwapWindow(window);_sdlec
		glClear(GL_COLOR_BUFFER_BIT);
	}
	// cleanup
	SDL_StopTextInput();
	free(chars);
	free(sprites);
	SDL_GL_DeleteContext(GLcontext);_sdlec
	SDL_Quit();_sdlec
	return 0;
}
