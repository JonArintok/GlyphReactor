
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>

#include "fileTools.h"
#include "optionsAndErrors.h"
#include "misc.h"

void glClearColori(uint32_t rgba) {
	glClearColor(
		(float) (rgba>>24)            /0xff, // red
		(float)((rgba>>16)&0x000000ff)/0xff, // green
		(float)((rgba>> 8)&0x000000ff)/0xff, // blue
		(float)((rgba    )&0x000000ff)/0xff  // alpha
	);
}


GLuint createShaderProgram(
	const char *vertPath,  // vertex shader
	const char *tecoPath,  // tessellation control shader
	const char *teevPath,  // tessellation evaluation shader
  const char *geomPath,  // geometry shader
  const char *fragPath,  // fragment shader
  const char *progName
) {
	if (!vertPath) {
    printf("error: no path for mandatory vertex shader\n");
    return 0;
	}
  int textBufSize = 1024;
	#define stageCount 5
  const char *paths[stageCount] = {
		vertPath, tecoPath, teevPath, geomPath, fragPath
	};
  GLenum shaderTypes[stageCount] = {
		GL_VERTEX_SHADER,
		GL_TESS_CONTROL_SHADER,
		GL_TESS_EVALUATION_SHADER,
		GL_GEOMETRY_SHADER,
		GL_FRAGMENT_SHADER
	};
	fr (i, stageCount) {
		if (!paths[i]) continue;
		int sourceSize = getFileSize(paths[i]);
	  if (!sourceSize) {
	    printf("error: could not open file \"%s\"\n", paths[i]);
	    return 0;
	  }
	  if (sourceSize > textBufSize) textBufSize = sourceSize + 1;
	}
  char *textBuf = malloc(textBufSize);
	GLuint shaderProgram = glCreateProgram();_glec
	GLint success = 0;
	fr (i, stageCount) {
		if (!paths[i]) continue;
		GLuint shader = glCreateShader(shaderTypes[i]);_glec
	  stringFromFile(paths[i], textBuf, textBufSize);
	  glShaderSource(shader, 1, (const GLchar * const*)&textBuf, NULL);_glec
	  glCompileShader(shader);_glec
	  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);_glec
	  if (!success) {
	    glGetShaderInfoLog(shader, textBufSize, NULL, textBuf);_glec
	    printf("error compiling shader \"%s\":\n%s\n", paths[i], textBuf);
	    return 0;
	  }
		glAttachShader(shaderProgram, shader);_glec
	}
  glLinkProgram(shaderProgram);_glec
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);_glec
  if (!success) {
    glGetShaderInfoLog(shaderProgram, textBufSize, NULL, textBuf);_glec
    printf("error linking shader program \"%s\":\n%s\n", progName, textBuf);
    return 0;
  }
  glValidateProgram(shaderProgram);_glec
  glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &success);_glec
  if (!success) {
    glGetShaderInfoLog(shaderProgram, textBufSize, NULL, textBuf);_glec
    printf("error: invalid shader program \"%s\":\n%s\n", progName, textBuf);
    return 0;
  }
  
  free(textBuf);
  return shaderProgram;
}

#include <SDL2/SDL_surface.h>

void texFromBmp(GLuint tex, const char *bmpPath) {
  glGenTextures(1, &tex);_glec
  glBindTexture(GL_TEXTURE_2D, tex);_glec
  SDL_Surface *srfcRaw = SDL_LoadBMP(bmpPath);_sdlec
  SDL_Surface *srfc  = SDL_ConvertSurfaceFormat(
    srfcRaw, SDL_PIXELFORMAT_ABGR8888, 0
  );_sdlec
  SDL_FreeSurface(srfcRaw);_sdlec
  glTexImage2D(
    GL_TEXTURE_2D,     // GLenum        target
    0,                 // GLint         level
    GL_RGBA,           // GLint         internalformat
    srfc->w,           // GLsizei       width
    srfc->h,           // GLsizei       height
    0,                 // GLint         border
    GL_RGBA,           // GLenum        format
    GL_UNSIGNED_BYTE,  // GLenum        type
    srfc->pixels       // const GLvoid *data
  );_glec
  SDL_FreeSurface(srfc);_sdlec
}
