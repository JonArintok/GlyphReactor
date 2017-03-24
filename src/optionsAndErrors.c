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
	#include <GL/glew.h>
	void glec(const int line, const char *file) {
	  GLenum GLstatus;
	  while ((GLstatus = glGetError()) != GL_NO_ERROR) {
			fprintf(LOG_GL_ERRORS_TO,
				"OpenGL error: %i on line %i in %s\n",
				GLstatus, line, file
			);
	  }
	}
#endif
