
#define LOG_SDL_ERRORS_TO  stderr
#define LOG_GL_ERRORS_TO   stderr
//#define LOG_EVENTS_TO      stderr
#define LOG_VERTEX_DATA_TO stderr
#define LOG_TIMING_TO      stderr

#define enableVSync 0

#ifdef LOG_SDL_ERRORS_TO
  void sdlec(int line, const char *file);
  #define _sdlec sdlec(__LINE__, __FILE__);
#else
  #define _sdlec
#endif

#ifdef LOG_GL_ERRORS_TO
  void glec(const int line, const char *file);
  #define _glec glec(__LINE__, __FILE__);
#else
  #define _glec
#endif

#define _SHOULD_NOT_BE_HERE_ \
	printf("!! SHOULD NOT BE HERE: line %i of %s\n", __LINE__, __FILE__)
// end macro
