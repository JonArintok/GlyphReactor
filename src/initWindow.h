#define  GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL.h>

extern SDL_Window   *window;
extern SDL_GLContext GLcontext;
extern int videoW;
extern int videoH;
#define videoHW_ (videoW/2)
#define videoHH_ (videoH/2)
#define scaleX_ (1.0/videoHW_)
#define scaleY_ (1.0/videoHH_)

int initWindow(void);
