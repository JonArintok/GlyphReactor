
#include "initWindow.h"
#include "optionsAndErrors.h"

int videoW = 1280;
int videoH =  800;
SDL_Window    *window    = NULL;
SDL_GLContext  GLcontext = NULL;
int initWindow(void) {
	SDL_Init(SDL_INIT_VIDEO);_sdlec
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	window = SDL_CreateWindow(
		"GlyphReactor",          // const char* title,
		SDL_WINDOWPOS_UNDEFINED, // int         x,
		SDL_WINDOWPOS_UNDEFINED, // int         y,
		videoW,                  // int         w,
		videoH,                  // int         h,
		SDL_WINDOW_OPENGL |      // Uint32      flags
		SDL_WINDOW_RESIZABLE
	);_sdlec
	GLcontext = SDL_GL_CreateContext(window);_sdlec
	SDL_GL_SetSwapInterval(enableVSync);_sdlec
	glewExperimental = GL_TRUE;
	{
		GLenum r = glewInit();
		if (r != GLEW_OK) {
			printf("GLEW error: %s\n", glewGetErrorString(r));
			return r;
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
	return 0;
}
