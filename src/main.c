
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
typedef struct {scoord s; tcoord t;} vert;          // textured vertex
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
void setCharVerts(vert *v, const scoord tlCorn, const char charIn) {
	v[0].s.x = tlCorn.x;
	v[0].s.y = tlCorn.y;
	v[0].s.z = tlCorn.z;
	v[0].t.u = texAtlGlyphPosX(charIn);
	v[0].t.v = texAtlGlyphPosY(charIn);
	v[1].s.x = tlCorn.x + texAtlGlyphW;
	v[1].s.y = tlCorn.y;
	v[1].s.z = tlCorn.z;
	v[1].t.u = texAtlGlyphPosX(charIn)+texAtlGlyphW;
	v[1].t.v = texAtlGlyphPosY(charIn);
	v[2].s.x = tlCorn.x;
	v[2].s.y = tlCorn.y - texAtlGlyphH;
	v[2].s.z = tlCorn.z;
	v[2].t.u = texAtlGlyphPosX(charIn);
	v[2].t.v = texAtlGlyphPosY(charIn)+texAtlGlyphH;
	v[3].s.x = tlCorn.x + texAtlGlyphW;
	v[3].s.y = tlCorn.y - texAtlGlyphH;
	v[3].s.z = tlCorn.z;
	v[3].t.u = texAtlGlyphPosX(charIn)+texAtlGlyphW;
	v[3].t.v = texAtlGlyphPosY(charIn)+texAtlGlyphH;
}
#define _identMat_ 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1
void setIdentMat(float mat[16]) {
	mat[ 0] = 1; mat[ 1] = 0; mat[ 2] = 0; mat[ 3] = 0;
	mat[ 4] = 0; mat[ 5] = 1; mat[ 6] = 0; mat[ 7] = 0;
	mat[ 8] = 0; mat[ 9] = 0; mat[10] = 1; mat[11] = 0;
	mat[12] = 0; mat[13] = 0; mat[14] = 0; mat[15] = 1;
};
void setScaleMat(float mat[16], float sx, float sy, float sz) {
	mat[ 0] *= sx;
	mat[ 5] *= sy;
	mat[10] *= sz;
}
void setTransMat(float mat[16], float sx, float sy, float sz) {
	mat[ 3] += sx;
	mat[ 7] += sy;
	mat[11] += sz;
}
void mulMat(float l[16], const float r[16]) {
	l[ 0] = l[ 0]*r[ 0] + l[ 1]*r[ 4] + l[ 2]*r[ 8] + l[ 3]*r[12];
	l[ 1] = l[ 0]*r[ 1] + l[ 1]*r[ 5] + l[ 2]*r[ 9] + l[ 3]*r[13];
	l[ 2] = l[ 0]*r[ 2] + l[ 1]*r[ 6] + l[ 2]*r[10] + l[ 3]*r[14];
	l[ 3] = l[ 0]*r[ 3] + l[ 1]*r[ 7] + l[ 2]*r[11] + l[ 3]*r[15];
	l[ 4] = l[ 4]*r[ 0] + l[ 5]*r[ 4] + l[ 6]*r[ 8] + l[ 7]*r[12];
	l[ 5] = l[ 4]*r[ 1] + l[ 5]*r[ 5] + l[ 6]*r[ 9] + l[ 7]*r[13];
	l[ 6] = l[ 4]*r[ 2] + l[ 5]*r[ 6] + l[ 6]*r[10] + l[ 7]*r[14];
	l[ 7] = l[ 4]*r[ 3] + l[ 5]*r[ 7] + l[ 6]*r[11] + l[ 7]*r[15];
	l[ 8] = l[ 8]*r[ 0] + l[ 9]*r[ 4] + l[10]*r[ 8] + l[11]*r[12];
	l[ 9] = l[ 8]*r[ 1] + l[ 9]*r[ 5] + l[10]*r[ 9] + l[11]*r[13];
	l[10] = l[ 8]*r[ 2] + l[ 9]*r[ 6] + l[10]*r[10] + l[11]*r[14];
	l[11] = l[ 8]*r[ 3] + l[ 9]*r[ 7] + l[10]*r[11] + l[11]*r[15];
	l[12] = l[12]*r[ 0] + l[13]*r[ 4] + l[14]*r[ 8] + l[15]*r[12];
	l[13] = l[12]*r[ 1] + l[13]*r[ 5] + l[14]*r[ 9] + l[15]*r[13];
	l[14] = l[12]*r[ 2] + l[13]*r[ 6] + l[14]*r[10] + l[15]*r[14];
	l[15] = l[12]*r[ 3] + l[13]*r[ 7] + l[14]*r[11] + l[15]*r[15];
}


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

//bool shiftIsDown(void) {
//	const SDL_Keymod km = SDL_GetModState();
//	return km&KMOD_LSHIFT || km&KMOD_RSHIFT || km&KMOD_CAPS;
//}



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
		"GlyphReactor",            //const char* title,
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
	const uint32_t chamCharCount = 5; // character widths between gun and word
	const uint32_t fileCharCount = getFileSize(txtPath);
	char chars[fileCharCount+chamCharCount];
	const uint32_t charCount = chamCharCount + cleanTxtFile(
		txtPath,
		chars+chamCharCount,
		fileCharCount
	);
	const uint32_t vertCount = charCount*4;
	const uint32_t indxCount = charCount*6;
	vert verts[vertCount];
	indx indxs[indxCount];
	uint32_t visCharBeg = chamCharCount;
	#define  visVertBeg_ (visCharBeg*4)
	#define  visIndxBeg_ (visCharBeg*6)
	uint32_t visCharEnd = charCount;
	#define  visVertEnd_ (visCharEnd*4)
	#define  visIndxEnd_ (visCharEnd*6)
	#define  visIndxCount_ (visIndxEnd_-visIndxBeg_)
	const scoord txtOrigin = {-halfVideoSize[0]/2, halfVideoSize[1]/2, 0};
	for (
		uint32_t cPos = visCharBeg, vPos = visVertBeg_, row = 0, col = 0;
		vPos < vertCount;
		cPos++, vPos+=4
	) {
		const scoord tlCorn = {
			txtOrigin.x + col*texAtlGlyphW,
			txtOrigin.y - row*texAtlGlyphH,
			1
		};
		setCharVerts(&verts[vPos], tlCorn, chars[cPos]);
		col++;
		if (chars[cPos] == delim) {
			row++;
			col = 0;
		}
	}
	for (
		uint32_t e = 0, v = 0;
		e < indxCount;
		v += 4, e += 6
	) {
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
	uint32_t curWord = 0;
	uint32_t frameWhenWordDropped = 0;
	#define wordDropEnvCount 128
	float wordDropEnv[wordDropEnvCount];
	fr(i,wordDropEnvCount) {
		wordDropEnv[i] = (float)texAtlGlyphH*((float)i/(float)wordDropEnvCount);
	}
	float transform[16] = {_identMat_};
	setScaleMat(transform, 1.0/halfVideoSize[0], 1.0/halfVideoSize[1], 1.0);
	
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
	GLint unif_texAtlSize    = glGetUniformLocation(shaderProgram, "texAtlSize");
  GLint unif_transform     = glGetUniformLocation(shaderProgram, "transform");
  glUniform2f(unif_texAtlSize, texAtlW, texAtlH);
	glUniformMatrix4fv(unif_transform, 1, 1, transform);
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
	uint32_t stuckCharCount = 0;
	uint32_t misBkspCount = 0;
  uint32_t curFrame = 0;
  bool running = true;
	glClear(GL_COLOR_BUFFER_BIT);
  glDrawElements(
		GL_TRIANGLES,
		visIndxCount_,
		GL_UNSIGNED_SHORT,
		(const GLvoid*)(visIndxBeg_*sizeof(indx))
	);_glec
	SDL_StartTextInput();
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
					#ifdef LOG_EVENTS_TO
					fprintf(LOG_EVENTS_TO,
						"KEYDOWN:\n\tscancode   : %s\n\tsymbol     : %s\n",
            SDL_GetScancodeName(event.key.keysym.scancode),
            SDL_GetKeyName(event.key.keysym.sym)
          );
					#endif
					if (event.key.keysym.sym == SDLK_BACKSPACE) {
						if (stuckCharCount) {
							stuckCharCount--;
							visCharBeg++;
							redraw = true;
						}
						else misBkspCount++;
					}
					break;
				case SDL_TEXTINPUT: {
					#ifdef LOG_EVENTS_TO
					fprintf(LOG_EVENTS_TO, "text input event: %s\n", event.text.text);
					#endif
					const char charIn = event.text.text[0];
					if (charIn == chars[visCharBeg] && !stuckCharCount) {
						if (charIn == delim) {
							curWord++;
							frameWhenWordDropped = curFrame;
						}
						visCharBeg++;
						redraw = true;
					}
					else {
						visCharBeg--;
						stuckCharCount++;
						if (visCharBeg < 0 || visCharBeg >= visCharEnd) {
							running = false;
							break;
						}
						redraw = true;
						scoord tlCorn = verts[visVertBeg_+4].s;
						tlCorn.x -= texAtlGlyphW;
						if (tlCorn.x <= txtOrigin.x-texAtlGlyphW*chamCharCount) {
							running = false;
						}
						else {
							chars[visCharBeg] = charIn;
							setCharVerts(&verts[visVertBeg_], tlCorn, charIn);
							glBufferSubData(
								GL_ARRAY_BUFFER,                    // GLenum        target
								visVertBeg_*sizeof(vert),           // GLintptr      offset​
								4*sizeof(vert),                     // GLsizeiptr    size​
								(const GLvoid*)&verts[visVertBeg_]  // const GLvoid *data
							);
						}
					}
          break;
				}
        case SDL_KEYUP:
					#ifdef LOG_EVENTS_TO
          fprintf(LOG_EVENTS_TO,
						"KEYUP:\n\tscancode: %s\n\tsymbol  : %s\n",
            SDL_GetScancodeName(event.key.keysym.scancode),
            SDL_GetKeyName(event.key.keysym.sym)
          );
					#endif
          break;
      }
    }
		if (!running) break;
		if (wordDropEnvCount > curFrame-frameWhenWordDropped) {
			float easing[16] = {_identMat_};
			setScaleMat(easing, 1.0/halfVideoSize[0], 1.0/halfVideoSize[1], 1.0);
			float transMat[16] = {_identMat_};
			setTransMat(
				transMat,
				0,
				curWord*texAtlGlyphH + wordDropEnv[curFrame-frameWhenWordDropped],
				0
			);
			mulMat(easing, transMat);
			glUniformMatrix4fv(unif_transform, 1, 1, easing);;
			redraw = true;
		}
    if (redraw) {
			glClear(GL_COLOR_BUFFER_BIT);
      glDrawElements(
				GL_TRIANGLES,
				visIndxCount_,
				GL_UNSIGNED_SHORT,
				(const GLvoid*)(visIndxBeg_*sizeof(indx))
			);_glec
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
	SDL_StopTextInput();
	SDL_GL_DeleteContext(GLcontext);_sdlec
	SDL_Quit();_sdlec
	return 0;
}
