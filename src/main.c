
#include <stdio.h>
#include <math.h>
#define  GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "optionsAndErrors.h"
#include "fileTools.h"
#include "cleanTxtFile.h"
#include "oglTools.h"
#include "sprite.h"
#include "misc.h"
#include "timestamp.h"
#include "../img/texAtlas.h"


int main(int argc, char **argv) {
	// init SDL and get an opengl window
	int videoW = 1280;
  int videoH =  800;
	#define videoHW_ (videoW/2)
  #define videoHH_ (videoH/2)
	#define scaleX_ (1.0/videoHW_)
	#define scaleY_ (1.0/videoHH_)
	SDL_Window    *window    = NULL;
	SDL_GLContext  GLcontext = NULL;
	SDL_Init(SDL_INIT_VIDEO);_sdlec
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
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
  	GLenum r = glewInit();
    if (r != GLEW_OK) {
      printf("GLEW error: %s\n", glewGetErrorString(r));
      return 1;
    }
  }
	#ifdef LOG_GL_ERRORS_TO
  fprintf(LOG_GL_ERRORS_TO,
		"OpenGL version: %s\n",
		glGetString(GL_VERSION)
	);
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(glErrorCallback, NULL);
	#endif
	
	// character data
	const char *txtPath       = "testFile.txt";
	const int   railLength    = 8; // character widths between gun and queue
	const int   fileCharCount = getFileSize(txtPath);
	int         visCharBeg    = railLength;
	int         charsSize     = visCharBeg + fileCharCount;
	char       *chars         = malloc(sizeof(char)*charsSize);
	sprite     *charSprites   = malloc(sizeof(sprite)*charsSize);
	const int   charCount     = cleanTxtFile(txtPath, &chars[visCharBeg], fileCharCount);
	int         visCharEnd    = visCharBeg + charCount;
	#define     visCharCount_ (visCharEnd-visCharBeg)
	
	// beam data
	const int beamCharPerWidth = 2; // affects kerning of beam glyphs
	const int beamSpritesSize  = (railLength+maxWordSize) * beamCharPerWidth;
	sprite   *beamSprites      = malloc(sizeof(sprite)*beamSpritesSize);
	
	// vert data spacing
	const int beamVertBeg = 0;
	const int charVertBeg = beamSpritesSize;
	const int vertBufSize = charsSize + beamSpritesSize;
	#define visCharVertBeg_ (charVertBeg+visCharBeg)
	
	// charSprites data
	#define txtOriginX_ (-railLength*texAtlGlyphW - 64)
	#define txtOriginY_ (videoHH_/4)
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
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
	// vertex buffer object
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(
    GL_ARRAY_BUFFER,
    vertBufSize*sizeof(sprite),
    NULL,
    GL_DYNAMIC_DRAW
  );
	glBufferSubData(
		GL_ARRAY_BUFFER,                        // GLenum        target
		visCharVertBeg_*sizeof(sprite),         // GLintptr      offset
		visCharCount_*sizeof(sprite),           // GLsizeiptr    size
		(const GLvoid*)&charSprites[visCharBeg] // const GLvoid *data
  );
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
  glUseProgram(shaderProgram);
	// attributes
  GLint attr_dstPosCntr = glGetAttribLocation(shaderProgram, "dstPosCntr");
	GLint attr_dstHlfSize = glGetAttribLocation(shaderProgram, "dstHlfSize");
	GLint attr_srcPosTpLt = glGetAttribLocation(shaderProgram, "srcPosTpLt");
	GLint attr_srcSize    = glGetAttribLocation(shaderProgram, "srcSize"   );
	GLint attr_mulColor   = glGetAttribLocation(shaderProgram, "mulColor"  );
  glEnableVertexAttribArray(attr_dstPosCntr);
	glEnableVertexAttribArray(attr_dstHlfSize);
	glEnableVertexAttribArray(attr_srcPosTpLt);
	glEnableVertexAttribArray(attr_srcSize   );
  glEnableVertexAttribArray(attr_mulColor  );
	const GLsizei vertAttrStride = sizeof(sprite);
	glVertexAttribPointer(attr_dstPosCntr, 2, GL_FLOAT,          GL_FALSE, vertAttrStride, (const GLvoid*) 0);
	glVertexAttribPointer(attr_dstHlfSize, 2, GL_FLOAT,          GL_FALSE, vertAttrStride, (const GLvoid*) 8);
  glVertexAttribPointer(attr_srcPosTpLt, 2, GL_UNSIGNED_SHORT, GL_FALSE, vertAttrStride, (const GLvoid*)16);
  glVertexAttribPointer(attr_srcSize,    2, GL_UNSIGNED_SHORT, GL_FALSE, vertAttrStride, (const GLvoid*)20);
  glVertexAttribPointer(attr_mulColor,   4, GL_UNSIGNED_SHORT, GL_TRUE,  vertAttrStride, (const GLvoid*)24);
	// uniforms
	GLint unif_texAtlSize   = glGetUniformLocation(shaderProgram, "texAtlSize");
	GLint unif_scale        = glGetUniformLocation(shaderProgram, "scale");
	GLint unif_translate    = glGetUniformLocation(shaderProgram, "translate");
	glUniform2f(unif_texAtlSize, texAtlW, texAtlH);
	glUniform2f(unif_scale, scaleX_, scaleY_);
	// texture
	GLuint texAtl = 0;
	texFromPng(texAtl, texAtlPath, false);
  glUniform1i(glGetUniformLocation(shaderProgram, "texAtl"), 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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
							glViewport(0, 0, videoW, videoH);
							glUniform2f(unif_scale, scaleX_, scaleY_);
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
				charSprites[visCharBeg] = charSprites[visCharBeg+1];
				charSprites[visCharBeg].dstCX -= texAtlGlyphW;
				if (charSprites[visCharBeg].dstCX <=  0.0 - texAtlGlyphW*railLength) {
					running = false;
					break;
				}
				else {
					chars[visCharBeg] = charEntered;
					charSprites[visCharBeg].dstHW = -texAtlGlyphW/2.0;
					charSprites[visCharBeg].dstHH = -texAtlGlyphH/2.0;
					charSprites[visCharBeg].srcX  = texAtlGlyphPosX(charEntered);
					charSprites[visCharBeg].srcY  = texAtlGlyphPosY(charEntered);
					charSprites[visCharBeg].mulR  = UINT16_MAX;
					charSprites[visCharBeg].mulG  = 0;
					charSprites[visCharBeg].mulB  = 0;
					charSprites[visCharBeg].mulO  = UINT16_MAX;
					glBufferSubData(
						GL_ARRAY_BUFFER,                        // GLenum        target
						visCharVertBeg_*sizeof(sprite),         // GLintptr      offset
						sizeof(sprite),                         // GLsizeiptr    size
						(const GLvoid*)&charSprites[visCharBeg] // const GLvoid *data
					);
				}
			}
		}
		// draw beam
		const float beamPhase = (float)(curFrame-frameWhenCharEntered)/beamGlowTime;
		if (beamPhase <= 1) {
			const int beamSize = (railLength + (visCharBeg-whereCurWordStarted))*beamCharPerWidth;
			fr (i, beamSize) {
				beamSprites[i].dstCX = -railLength*texAtlGlyphW + i*texAtlGlyphW/beamCharPerWidth;
				beamSprites[i].dstCY = 0;
				beamSprites[i].dstHW = texAtlGlyphW/2.0;
				beamSprites[i].dstHH = texAtlGlyphH/2.0;
				beamSprites[i].srcX  = texAtlGlyphPosX(lastCharEntered);
				beamSprites[i].srcY  = texAtlGlyphPosY(lastCharEntered);
				beamSprites[i].srcW  = texAtlGlyphW;
				beamSprites[i].srcH  = texAtlGlyphH;
				beamSprites[i].mulR  = 0;
				beamSprites[i].mulG  = UINT16_MAX;
				beamSprites[i].mulB  = UINT16_MAX;
				beamSprites[i].mulO  = UINT16_MAX * (1.0 - beamPhase);
			}
			#ifdef LOG_VERTEX_DATA_TO
			fprintf(LOG_VERTEX_DATA_TO, "\nBEAM\n");
			printSprites(beamSprites, beamSize, __LINE__);
			#endif
			glBufferSubData(
				GL_ARRAY_BUFFER,             // GLenum        target
				beamVertBeg*sizeof(sprite),  // GLintptr      offset
				beamSize*sizeof(sprite),     // GLsizeiptr    size
				(const GLvoid*)beamSprites   // const GLvoid *data
			);
			glUniform2f(unif_translate, txtOriginX_, txtOriginY_);
			glDrawArrays(GL_POINTS, 0, beamSize);
		}
		// draw word queue
		if (wordDropEnvCount > curFrame-frameWhenWordDropped) {
			glUniform2f(
				unif_translate,
				txtOriginX_,
				txtOriginY_ + curWord*texAtlGlyphH - wordDropEnv[curFrame-frameWhenWordDropped]
			);
		}
		else {
			glUniform2f(
				unif_translate,
				txtOriginX_,
				txtOriginY_ + curWord*texAtlGlyphH
			);
		}
    glDrawArrays(GL_POINTS, visCharVertBeg_, visCharCount_);
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
	free(charSprites);
	free(beamSprites);
	SDL_GL_DeleteContext(GLcontext);_sdlec
	SDL_Quit();_sdlec
	return 0;
}
