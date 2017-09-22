
#include <stdio.h>

#define LOG_SDL_ERRORS_TO  stdout
#define LOG_GL_ERRORS_TO   stdout
//#define LOG_VERTEX_DATA_TO stdout
//#define LOG_AUDIOSPEC_TO   stdout
//#define LOG_TIMING_TO      stdout

#define enableVSync   1
#define drawWireFrame 0

#define visSpirosSize 8


#ifdef LOG_SDL_ERRORS_TO
void sdlec(int line, const char *file);
#define _sdlec sdlec(__LINE__, __FILE__);
#else
#define _sdlec
#endif

#ifdef LOG_GL_ERRORS_TO
#define  GLEW_STATIC
#include <GL/glew.h>
void glErrorCallback(
	GLenum        source,
	GLenum        type,
	GLuint        id,
	GLenum        severity,
	GLsizei       length,
	const GLchar *message,
	const void   *userParam
);
#endif

#define _SHOULD_NOT_BE_HERE_ printf("!! SHOULD NOT BE HERE: line %i of %s\n", __LINE__, __FILE__)
