
#define  GLEW_STATIC
#include <GL/glew.h>

extern int beamVertBeg;
extern int charVertBeg;
extern int spiroVertBeg;
extern GLint unif_texAtlSize;
extern GLint unif_scale;
extern GLint unif_translate;
#define visCharVertBeg_ (charVertBeg+visCharBeg)
int initOpenGl(void);
