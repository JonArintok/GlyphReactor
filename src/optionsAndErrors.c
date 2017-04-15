#include "optionsAndErrors.h"

#ifdef LOG_SDL_ERRORS_TO
#include <SDL2/SDL_error.h>
void sdlec(int line, const char *file) {
  const char *error = SDL_GetError();
  if (!error || !error[0]) return;
	fprintf(LOG_SDL_ERRORS_TO,
		"SDL error at line %i in %s :\n%s",
		line, file, error
	);
  SDL_ClearError();
  exit(-10);
}
#endif

#ifdef LOG_GL_ERRORS_TO
void glec(const int line, const char *file) {
  GLenum GLstatus;
  while ((GLstatus = glGetError()) != GL_NO_ERROR) {
		fprintf(LOG_GL_ERRORS_TO,
			"OpenGL error: %i on line %i in %s\n",
			GLstatus, line, file
		);
  }
}
static const char* Source(GLenum source) {
	switch(source) {
    case GL_DEBUG_SOURCE_API_ARB             : return "API";
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB   : return "WINDOW_SYSTEM";
    case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB : return "SHADER_COMPILER";
    case GL_DEBUG_SOURCE_THIRD_PARTY_ARB     : return "THIRD_PARTY";
    case GL_DEBUG_SOURCE_APPLICATION_ARB     : return "APPLICATION";
    case GL_DEBUG_SOURCE_OTHER_ARB           : return "OTHER";
    default                                  : return "not recognized";
  }
}
static const char* Type(GLenum type) {
  switch(type) {
	  case GL_DEBUG_TYPE_ERROR_ARB               : return "ERROR";
	  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB : return "DEPRECATED_BEHAVIOR";
	  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB  : return "UNDEFINED_BEHAVIOR";
	  case GL_DEBUG_TYPE_PORTABILITY_ARB         : return "PORTABILITY";
	  case GL_DEBUG_TYPE_PERFORMANCE_ARB         : return "PERFORMANCE";
	  case GL_DEBUG_TYPE_OTHER_ARB               : return "OTHER";
	  default                                    : return "not recognized";
  }
}
static const char* Severity(GLenum severity) {
  switch(severity) {
    case GL_DEBUG_SEVERITY_HIGH_ARB   : return "HIGH";
    case GL_DEBUG_SEVERITY_MEDIUM_ARB : return "MEDIUM";
    case GL_DEBUG_SEVERITY_LOW_ARB    : return "LOW";
		default                           : return "not recognized";
	}
}
void glErrorCallback(
	GLenum        source,
	GLenum        type,
	GLuint        id,
	GLenum        severity,
	GLsizei       length,
	const GLchar *message,
	const void   *userParam
) {
	if (severity == 33387) return; // don't know what this is but it's persistent
	fprintf(LOG_GL_ERRORS_TO,
		"GL ERROR\n"
		"\tsource  : %i: %s\n"
		"\ttype    : %i: %s\n"
		"\tid      : %i\n"
		"\tseverity: %i: %s\n"
		"\tmessage : %s\n",
		source, Source(source),
		type, Type(type),
		id,
		severity, Severity(severity),
		message
	);
}
#endif
