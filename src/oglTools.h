#include <stdbool.h>

void glClearColori(uint32_t c);

GLuint createShaderProgram(
	const char *vertPath,  // vertex shader
	const char *tecoPath,  // tessellation control shader
	const char *teevPath,  // tessellation evaluation shader
  const char *geomPath,  // geometry shader
  const char *fragPath,  // fragment shader
  const char *progName
);

void texFromBmp(GLuint tex, const char *bmpPath);
void texFromPng(GLuint tex, const char *pngPath, bool mirrorY);
