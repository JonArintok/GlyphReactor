
#include "initOpenGl.h"
#include "../img/texAtlas.h"
#include "sprite.h"
#include "initSprites.h"
#include "oglTools.h"
#include "initWindow.h"

int beamVertBeg;
int charVertBeg;
int vertBufSize;
GLint unif_texAtlSize;
GLint unif_scale;
GLint unif_translate;
int initOpenGl(void) {
	beamVertBeg = 0;
	charVertBeg = beamSpritesSize;
	vertBufSize = charsSize + beamSpritesSize;
	// vertex array object
	GLuint vao;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	// vertex buffer object
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(
	  GL_ARRAY_BUFFER,
	  vertBufSize*sizeof(sprite),
	  NULL,
	  GL_DYNAMIC_DRAW
	);
	glBufferSubData(
		GL_ARRAY_BUFFER,                        // GLenum        target
		visCharVertBeg_*sizeof(sprite),         // GLintptr      offset
		visCharCount_*sizeof(sprite),           // GLsizeiptr    size
		(const GLvoid*)&charSprites[visCharBeg] // const GLvoid *data
	);
	// shader program
	GLuint shaderProgram = createShaderProgram(
	  "src/shadeVert.glsl",
		NULL,
		NULL,
		"src/shadeGeom.glsl",
	  "src/shadeFrag.glsl",
	  "shaderProgram"
	);
	if (!shaderProgram) return __LINE__;
	glUseProgram(shaderProgram);
	// attributes
	GLint attr_dstPosCntr = glGetAttribLocation(shaderProgram, "dstPosCntr");
	GLint attr_dstHlfSize = glGetAttribLocation(shaderProgram, "dstHlfSize");
	GLint attr_srcPosTpLt = glGetAttribLocation(shaderProgram, "srcPosTpLt");
	GLint attr_srcSize    = glGetAttribLocation(shaderProgram, "srcSize"   );
	GLint attr_mulColor   = glGetAttribLocation(shaderProgram, "mulColor"  );
	glEnableVertexAttribArray(attr_dstPosCntr);
	glEnableVertexAttribArray(attr_dstHlfSize);
	glEnableVertexAttribArray(attr_srcPosTpLt);
	glEnableVertexAttribArray(attr_srcSize   );
	glEnableVertexAttribArray(attr_mulColor  );
	const GLsizei vertAttrStride = sizeof(sprite);
	glVertexAttribPointer(attr_dstPosCntr, 2, GL_FLOAT,          GL_FALSE, vertAttrStride, (const GLvoid*) 0);
	glVertexAttribPointer(attr_dstHlfSize, 2, GL_FLOAT,          GL_FALSE, vertAttrStride, (const GLvoid*) 8);
	glVertexAttribPointer(attr_srcPosTpLt, 2, GL_UNSIGNED_SHORT, GL_FALSE, vertAttrStride, (const GLvoid*)16);
	glVertexAttribPointer(attr_srcSize,    2, GL_UNSIGNED_SHORT, GL_FALSE, vertAttrStride, (const GLvoid*)20);
	glVertexAttribPointer(attr_mulColor,   4, GL_UNSIGNED_SHORT, GL_TRUE,  vertAttrStride, (const GLvoid*)24);
	// uniforms
	unif_texAtlSize   = glGetUniformLocation(shaderProgram, "texAtlSize");
	unif_scale        = glGetUniformLocation(shaderProgram, "scale");
	unif_translate    = glGetUniformLocation(shaderProgram, "translate");
	glUniform2f(unif_texAtlSize, texAtlW, texAtlH);
	glUniform2f(unif_scale, scaleX_, scaleY_);
	// texture
	GLuint texAtl = 0;
	texFromPng(texAtl, texAtlPath, false);
	glUniform1i(glGetUniformLocation(shaderProgram, "texAtl"), 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// other opengl
	const uint32_t clearColor = 0x224455ff; // rgba
	glClearColori(clearColor);
	glClear(GL_COLOR_BUFFER_BIT);
	#if drawWireFrame
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	#endif
	return 0;
}
