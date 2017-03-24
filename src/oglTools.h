
void glClearColori(uint32_t c);

GLuint createShaderProgram(
  const char *vertPath,
  const char *fragPath,
  const char *progName
);

GLuint texFromBmp(const char *bmpPath);
