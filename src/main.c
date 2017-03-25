
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


typedef struct {float x; float y; float z;} scoord; // spacial coordinate
typedef struct {int16_t u; int16_t v;} tcoord;      // texture coordinate
typedef struct {scoord s; tcoord t;} vert;         // textured vertex
typedef uint16_t indx;

#ifdef LOG_VERTEX_DATA_TO
void printVert(vert v) {
	fprintf(LOG_VERTEX_DATA_TO,
		"x: %7.2f  y: %7.2f  z: %7.2f   u: %5i  v: %5i\n",
		v.s.x, v.s.y, v.s.z, v.t.u, v.t.v
	);
}
void printVerts(vert *v, uint32_t count) {
	fprintf(LOG_VERTEX_DATA_TO, "%i verts\n", count);
	fr(i,count) {printf("%5i:  ", i); printVert(v[i]);}
}
void printIndxs(indx *in, uint32_t count) {
	fprintf(LOG_VERTEX_DATA_TO, "%i indxs\n", count);
	fr(i,count) {printf("%5i:  ", i); printf("%5i\n", in[i]);}
}
#endif


const char delim = ' ';
uint32_t cleanTxtFile(
	const char *txtPath,
	char       *chars,
	uint32_t    writeLimit
) {
	stringFromFile(txtPath, chars, writeLimit);
	uint32_t readPos = 0;
	uint32_t writPos = 0;
	while(true) {
		if (chars[readPos] == '\0') {
			chars[writPos] = '\0';
			break;
		}
		if (chars[readPos] < '!' || chars[readPos] > '~') {
			readPos++;
			if (chars[writPos-1] != delim) {
				chars[writPos] = delim;
				writPos++;
			}
			continue;
		}
		chars[writPos] = chars[readPos];
		readPos++;
		writPos++;
		if (readPos > writeLimit) _SHOULD_NOT_BE_HERE_;
	}
	return writPos;
}


int main(int argc, char **argv) {
  int16_t videoSize[2] = {800, 600};
  int16_t halfVideoSize[2] = {videoSize[0]/2, videoSize[1]/2};
	SDL_Window    *window    = NULL;
	SDL_GLContext  GLcontext = NULL;
	SDL_Init(SDL_INIT_VIDEO);_sdlec
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	window = SDL_CreateWindow(
		"AntiGlyph",               //const char* title,
		SDL_WINDOWPOS_UNDEFINED,   //int         x,
		SDL_WINDOWPOS_UNDEFINED,   //int         y,
		videoSize[0],              //int         w,
		videoSize[1],              //int         h,
		SDL_WINDOW_OPENGL          //Uint32      flags
	);_sdlec
	GLcontext = SDL_GL_CreateContext(window);_sdlec
  SDL_GL_SetSwapInterval(1);_sdlec
  glewExperimental = GL_TRUE;
  {
  	GLenum r = glewInit();//_glec
		// flush GL_INVALID_ENUM persistent error...
		if (glGetError() == GL_INVALID_ENUM) {}_glec
    if (r != GLEW_OK) {
      printf("GLEW error: %s\n", glewGetErrorString(r));
      return 1;
    }
  }
  //printf("OpenGL version: %s\n\n", glGetString(GL_VERSION));_glec
	
	const char *txtPath = "testFile.txt";
	const uint32_t fileCharCount = getFileSize(txtPath);
	char chars[fileCharCount];
	const uint32_t charCount = cleanTxtFile(txtPath, chars, fileCharCount);
	const uint32_t vertCount = charCount*4;
	const uint32_t indxCount = charCount*6;
	vert verts[vertCount];
	indx indxs[indxCount];
	
	const scoord txtOrigin = {-halfVideoSize[0]/2, halfVideoSize[1]/2, 0};
	for (
		uint32_t cPos = 0, vPos = 0, row = 0, col = 0;
		vPos < vertCount;
		cPos++, vPos+=4
	) {
		verts[vPos  ].s.x = txtOrigin.x + col*texAtlGlyphW;
		verts[vPos  ].s.y = txtOrigin.y - row*texAtlGlyphH;
		verts[vPos  ].s.z = 0;
		verts[vPos  ].t.u = texAtlGlyphPosX(chars[cPos]);
		verts[vPos  ].t.v = texAtlGlyphPosY(chars[cPos]);
		verts[vPos+1].s.x = txtOrigin.x + (col+1)*texAtlGlyphW;
		verts[vPos+1].s.y = txtOrigin.y - row*texAtlGlyphH;
		verts[vPos+1].s.z = 0;
		verts[vPos+1].t.u = texAtlGlyphPosX(chars[cPos])+texAtlGlyphW;
		verts[vPos+1].t.v = texAtlGlyphPosY(chars[cPos]);
		verts[vPos+2].s.x = txtOrigin.x + col*texAtlGlyphW;
		verts[vPos+2].s.y = txtOrigin.y - (row+1)*texAtlGlyphH;
		verts[vPos+2].s.z = 0;
		verts[vPos+2].t.u = texAtlGlyphPosX(chars[cPos]);
		verts[vPos+2].t.v = texAtlGlyphPosY(chars[cPos])+texAtlGlyphH;
		verts[vPos+3].s.x = txtOrigin.x + (col+1)*texAtlGlyphW;
		verts[vPos+3].s.y = txtOrigin.y - (row+1)*texAtlGlyphH;
		verts[vPos+3].s.z = 0;
		verts[vPos+3].t.u = texAtlGlyphPosX(chars[cPos])+texAtlGlyphW;
		verts[vPos+3].t.v = texAtlGlyphPosY(chars[cPos])+texAtlGlyphH;
		col++;
		if (chars[cPos] == delim) {
			row++;
			col = 0;
		}
	}
	for (uint32_t e = 0, v = 0; e < indxCount; v += 4, e += 6) {
    indxs[e  ] = v;
    indxs[e+1] = v+1;
    indxs[e+2] = v+2;
    indxs[e+3] = v+1;
    indxs[e+4] = v+3;
    indxs[e+5] = v+2;
	}
	
	#ifdef LOG_VERTEX_DATA_TO
	printVerts(verts, vertCount);
	printIndxs(indxs, indxCount);
	#endif
	
	//uint32_t charPos    = 0;
	//uint32_t visCharBeg = 0;
	//uint32_t visCharEnd = 0;
	
  GLuint vao;
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glGenVertexArrays(1, &vao);_glec
  glBindVertexArray(vao);_glec
  GLuint vbo;
  GLuint ebo;
  glGenBuffers(1, &vbo);_glec
  glGenBuffers(1, &ebo);_glec
  glBindBuffer(GL_ARRAY_BUFFER,         vbo);_glec
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);_glec
  glBufferData(
    GL_ARRAY_BUFFER,
    vertCount*sizeof(vert),
    verts,
    GL_STATIC_DRAW
  );_glec
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    indxCount*sizeof(indx),
    indxs,
    GL_STATIC_DRAW
  );_glec
  GLuint shaderProgram = createShaderProgram(
    "src/shadeVert.glsl",
    "src/shadeFrag.glsl",
    "shaderProgram"
  );
  if (!shaderProgram) return __LINE__;
  glUseProgram(shaderProgram);_glec
  GLint attr_pos      = glGetAttribLocation(shaderProgram, "pos"  );_glec
  GLint attr_texCoord = glGetAttribLocation(shaderProgram, "texCoord");_glec
  glEnableVertexAttribArray(attr_pos  );_glec
  glEnableVertexAttribArray(attr_texCoord);_glec
  glVertexAttribPointer(attr_pos,      3, GL_FLOAT, GL_FALSE, 16, (const GLvoid*)  0);_glec
  glVertexAttribPointer(attr_texCoord, 2, GL_SHORT, GL_FALSE, 16, (const GLvoid*) 12);_glec
  GLint unif_halfVideoSize = glGetUniformLocation(shaderProgram, "halfVideoSize");
  GLint unif_texAtlSize    = glGetUniformLocation(shaderProgram, "texAtlSize");
  glUniform2f(unif_halfVideoSize, halfVideoSize[0], halfVideoSize[1]);
  glUniform2f(unif_texAtlSize, texAtlW, texAtlH);
  /*GLuint tex = */texFromBmp(texAtlPath);
  glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);_glec
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);_glec
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	const uint32_t clearColor = 0x4488bbff; //rgba
	glClearColori(clearColor);
  timestamp ts_oldFrameStart={0,0},ts_newFrameStart={0,0},ts_frameDelta={0,0};
	#ifdef LOG_TIMING_TO
  timestamp ts_compTime = {0,0}, ts_now = {0,0};
  #endif
  clock_gettime(CLOCK_MONOTONIC, &ts_newFrameStart);
  int curFrame = 0;
  bool running = true;
	glClear(GL_COLOR_BUFFER_BIT);
  glDrawElements(GL_TRIANGLES, indxCount, GL_UNSIGNED_SHORT, 0);_glec
	while (running) {
    ts_oldFrameStart = ts_newFrameStart;
    clock_gettime(CLOCK_MONOTONIC, &ts_newFrameStart);
    getTimeDelta(&ts_oldFrameStart, &ts_newFrameStart, &ts_frameDelta);
    #ifdef LOG_TIMING_TO
    fprintf(LOG_TIMING_TO,
      "ts_frameDelta: %1ld s, %9ld ns\n",
      ts_frameDelta.tv_sec, ts_frameDelta.tv_nsec
    );
    #endif
    bool redraw = false;
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT: running = false; break;
        case SDL_KEYDOWN:
          printf("KEYDOWN:\n\tscancode: %s\n\tsymbol  : %s\n",
            SDL_GetScancodeName(event.key.keysym.scancode),
            SDL_GetKeyName(event.key.keysym.sym)
          );
          break;
        case SDL_KEYUP:
          printf("KEYUP:\n\tscancode: %s\n\tsymbol  : %s\n",
            SDL_GetScancodeName(event.key.keysym.scancode),
            SDL_GetKeyName(event.key.keysym.sym)
          );
          break;
      }
    }
    if (redraw) {
			glClear(GL_COLOR_BUFFER_BIT);
      glDrawElements(GL_TRIANGLES, indxCount, GL_UNSIGNED_SHORT, 0);_glec
      redraw = false;
    }
    #ifdef LOG_TIMING_TO
		clock_gettime(CLOCK_MONOTONIC, &ts_now);
    getTimeDelta(&ts_newFrameStart, &ts_now, &ts_compTime);
    fprintf(LOG_TIMING_TO,
			"ts_compTime: %3ld s, %9ld ns\n",
      ts_compTime.tv_sec, ts_compTime.tv_nsec
    );
    #endif
		SDL_GL_SwapWindow(window);_sdlec
    curFrame++;
	}
	SDL_GL_DeleteContext(GLcontext);_sdlec
	SDL_Quit();_sdlec
	return 0;
}
