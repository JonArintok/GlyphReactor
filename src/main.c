
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
	fr(i,count) {printf("%5i:  ", i); fflush(stdout); printVert(v[i]);}
}
void printIndxs(indx *in, uint32_t count) {
	fprintf(LOG_VERTEX_DATA_TO, "%i indxs\n", count);
	fr(i,count) printf("%5i: %5i\n", i, in[i]);
}
#endif

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
void mulMat(float dest[16], const float l[16], const float r[16]) {
	dest[ 0] = l[ 0]*r[ 0] + l[ 1]*r[ 4] + l[ 2]*r[ 8] + l[ 3]*r[12];
	dest[ 1] = l[ 0]*r[ 1] + l[ 1]*r[ 5] + l[ 2]*r[ 9] + l[ 3]*r[13];
	dest[ 2] = l[ 0]*r[ 2] + l[ 1]*r[ 6] + l[ 2]*r[10] + l[ 3]*r[14];
	dest[ 3] = l[ 0]*r[ 3] + l[ 1]*r[ 7] + l[ 2]*r[11] + l[ 3]*r[15];
	dest[ 4] = l[ 4]*r[ 0] + l[ 5]*r[ 4] + l[ 6]*r[ 8] + l[ 7]*r[12];
	dest[ 5] = l[ 4]*r[ 1] + l[ 5]*r[ 5] + l[ 6]*r[ 9] + l[ 7]*r[13];
	dest[ 6] = l[ 4]*r[ 2] + l[ 5]*r[ 6] + l[ 6]*r[10] + l[ 7]*r[14];
	dest[ 7] = l[ 4]*r[ 3] + l[ 5]*r[ 7] + l[ 6]*r[11] + l[ 7]*r[15];
	dest[ 8] = l[ 8]*r[ 0] + l[ 9]*r[ 4] + l[10]*r[ 8] + l[11]*r[12];
	dest[ 9] = l[ 8]*r[ 1] + l[ 9]*r[ 5] + l[10]*r[ 9] + l[11]*r[13];
	dest[10] = l[ 8]*r[ 2] + l[ 9]*r[ 6] + l[10]*r[10] + l[11]*r[14];
	dest[11] = l[ 8]*r[ 3] + l[ 9]*r[ 7] + l[10]*r[11] + l[11]*r[15];
	dest[12] = l[12]*r[ 0] + l[13]*r[ 4] + l[14]*r[ 8] + l[15]*r[12];
	dest[13] = l[12]*r[ 1] + l[13]*r[ 5] + l[14]*r[ 9] + l[15]*r[13];
	dest[14] = l[12]*r[ 2] + l[13]*r[ 6] + l[14]*r[10] + l[15]*r[14];
	dest[15] = l[12]*r[ 3] + l[13]*r[ 7] + l[14]*r[11] + l[15]*r[15];
}


const char delim = ' ';
int cleanTxtFile(
	const char *txtPath,
	char       *chars,
	uint32_t    writeLimit
) {
	int readLimit = stringFromFile(txtPath, chars, writeLimit);
	int readPos = 0;
	int writPos = 0;
	while(true) {
		if (chars[readPos] == '\0' || readPos > readLimit) {
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
	int videoW = 1280;
  int videoH =  720;
	#define halfVideoW_ (videoW/2)
  #define halfVideoH_ (videoH/2)
	#define scaleX_ (1.0/halfVideoW_)
	#define scaleY_ (1.0/halfVideoH_)
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
  fprintf(LOG_GL_ERRORS_TO, "OpenGL version: %s\n", glGetString(GL_VERSION));_glec
	#endif
	
	const char *txtPath = "testFile.txt";
	const int chamCharCount = 8; // character widths between gun and word
	const int fileCharCount = getFileSize(txtPath);
	char chars[fileCharCount+chamCharCount];
	const int charCount = chamCharCount + cleanTxtFile(
		txtPath,
		chars+chamCharCount,
		fileCharCount
	);
	vert verts[charCount];
	int visCharBeg = chamCharCount;
	int visCharEnd = charCount;
	#define  visCharCount_ (visCharEnd-visCharBeg)
	#define  txtOriginX_ ((-halfVideoW_/2)+(chamCharCount*texAtlGlyphW)+64)
	#define  txtOriginY_ (halfVideoH_/4)
	for (int cPos = visCharBeg, row = 0, col = 0; cPos < charCount; cPos++) {
		verts[cPos].s.x = 0.0 + col*texAtlGlyphW;
		verts[cPos].s.y = 0.0 - row*texAtlGlyphH;
		verts[cPos].s.z = 0.0;
		verts[cPos].t.u = texAtlGlyphPosX(chars[cPos]);
		verts[cPos].t.v = texAtlGlyphPosY(chars[cPos]);
		col++;
		if (chars[cPos] == delim) {
			row++;
			col = 0;
		}
	}
	#ifdef LOG_VERTEX_DATA_TO
	printVerts(verts, charCount);
	#endif
	int curWord = 0;
	int frameWhenWordDropped = 0;
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
	
  GLuint vao;
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glGenVertexArrays(1, &vao);_glec
  glBindVertexArray(vao);_glec
  GLuint vbo;
  glGenBuffers(1, &vbo);_glec
  glBindBuffer(GL_ARRAY_BUFFER, vbo);_glec
  glBufferData(
    GL_ARRAY_BUFFER,
    charCount*sizeof(vert),
    verts,
    GL_STATIC_DRAW
  );_glec
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
  GLint attr_pos      = glGetAttribLocation(shaderProgram, "pos"  );_glec
	GLint attr_texCoord = glGetAttribLocation(shaderProgram, "texCoord");_glec
  glEnableVertexAttribArray(attr_pos  );_glec
  glEnableVertexAttribArray(attr_texCoord);_glec
  glVertexAttribPointer(attr_pos,      3, GL_FLOAT, GL_FALSE, 16, (const GLvoid*)  0);_glec
  glVertexAttribPointer(attr_texCoord, 2, GL_SHORT, GL_FALSE, 16, (const GLvoid*) 12);_glec
	GLint unif_texAtlSize   = glGetUniformLocation(shaderProgram, "texAtlSize");_glec
	GLint unif_transform    = glGetUniformLocation(shaderProgram, "transform");_glec
	GLint unif_glyphTexSize = glGetUniformLocation(shaderProgram, "glyphTexSize");_glec
  GLint unif_glyphSpaSize = glGetUniformLocation(shaderProgram, "glyphSpaSize");_glec
	glUniform2f(unif_texAtlSize, texAtlW, texAtlH);_glec
	glUniform2f(unif_glyphTexSize, (float)texAtlGlyphW/(float)texAtlW, (float)texAtlGlyphH/(float)texAtlH);_glec
	glUniform2f(unif_glyphSpaSize, (float)texAtlGlyphW*(float)scaleX_, (float)texAtlGlyphH*(float)scaleY_);_glec
	GLuint tex = 0;
	texFromBmp(tex, texAtlPath);
  glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0);_glec
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);_glec
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);_glec
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);_glec
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);_glec
	const uint32_t clearColor = 0x4488bbff; // rgba
	glClearColori(clearColor);
	if (drawWireFrame) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
  timestamp ts_oldFrameStart={0,0},ts_newFrameStart={0,0},ts_frameDelta={0,0};
	#ifdef LOG_TIMING_TO
  timestamp ts_compTime = {0,0}, ts_now = {0,0};
  #endif
  clock_gettime(CLOCK_MONOTONIC, &ts_newFrameStart);
	
	int stuckCharCount = 0;
	int misBkspCount = 0;
	int curFrame = 0;
  
	bool running = true;
	glClear(GL_COLOR_BUFFER_BIT);
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
		char charEntered = '\0';
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT: running = false; break;
				case SDL_WINDOWEVENT:
					switch (event.window.event) {
						case SDL_WINDOWEVENT_RESIZED:
							#ifdef LOG_EVENTS_TO
							fprintf(LOG_EVENTS_TO,
								"Window %d resized to %dx%d\n",
                event.window.windowID,
								event.window.data1,
								event.window.data2
							);
							#endif
							videoW = event.window.data1;
							videoH = event.window.data2;
							glViewport(0, 0, videoW, videoH);
            break;
					}
					break;
        case SDL_KEYDOWN:
					#ifdef LOG_EVENTS_TO
					fprintf(LOG_EVENTS_TO,
						"KEYDOWN:\n\tscancode   : %s\n\tsymbol     : %s\n",
            SDL_GetScancodeName(event.key.keysym.scancode),
            SDL_GetKeyName(event.key.keysym.sym)
          );
					#endif
					switch (event.key.keysym.sym) {
						case SDLK_BACKSPACE: charEntered = SDLK_BACKSPACE; break;
						case SDLK_ESCAPE:    running = false; break;
					}
					break;
				case SDL_TEXTINPUT: {
					#ifdef LOG_EVENTS_TO
					fprintf(LOG_EVENTS_TO, "text input event: %s\n", event.text.text);
					#endif
					charEntered = event.text.text[0];
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
		if (charEntered) {
			if (charEntered == SDLK_BACKSPACE) {
				if (stuckCharCount) {
					stuckCharCount--;
					visCharBeg++;
				}
				else misBkspCount++;
			}
			else if (charEntered == chars[visCharBeg] && !stuckCharCount) {
				if (charEntered == delim) {
					curWord++;
					frameWhenWordDropped = curFrame;
				}
				visCharBeg++;
			}
			else {
				visCharBeg--;
				stuckCharCount++;
				if (visCharBeg < 0 || visCharBeg >= visCharEnd) {
					running = false;
					break;
				}
				scoord tlCorn = verts[visCharBeg+1].s;
				tlCorn.x -= texAtlGlyphW;
				if (tlCorn.x <=  0.0 - texAtlGlyphW*chamCharCount) {
					running = false;
				}
				else {
					chars[visCharBeg] = charEntered;
					verts[visCharBeg].s.x = tlCorn.x;
					verts[visCharBeg].s.y = tlCorn.y;
					verts[visCharBeg].s.z = tlCorn.z;
					verts[visCharBeg].t.u = texAtlGlyphPosX(charEntered);
					verts[visCharBeg].t.v = texAtlGlyphPosY(charEntered);
					glBufferSubData(
						GL_ARRAY_BUFFER,                    // GLenum        target
						visCharBeg*sizeof(vert),           // GLintptr      offset​
						sizeof(vert),                       // GLsizeiptr    size​
						(const GLvoid*)&verts[visCharBeg]  // const GLvoid *data
					);
				}
			}
		}
		float scaleMat[16] = {_identMat_};
		float transMat[16] = {_identMat_};
		float finalMat[16] = {_identMat_};
		if (wordDropEnvCount > curFrame-frameWhenWordDropped) {
			setTransMat(
				transMat,
				txtOriginX_,
				txtOriginY_ + curWord*texAtlGlyphH - wordDropEnv[curFrame-frameWhenWordDropped],
				0
			);
		}
		else {
			setTransMat(
				transMat,
				txtOriginX_,
				txtOriginY_ + curWord*texAtlGlyphH,
				0
			);
		}
		setScaleMat(scaleMat, scaleX_, scaleY_, 1.0);
		mulMat(finalMat, scaleMat, transMat);
		glUniformMatrix4fv(unif_transform, 1, 1, finalMat);
		glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(
			GL_POINTS,
			visCharBeg,
			visCharCount_
		);_glec
		
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
