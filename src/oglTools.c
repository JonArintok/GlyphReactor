
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>

#include "fileTools.h"
#include "optionsAndErrors.h"
#include "misc.h"

void glClearColori(uint32_t rgba) {
	glClearColor(
		(float) (rgba>>24)            /0xff, // red
		(float)((rgba>>16)&0x000000ff)/0xff, // green
		(float)((rgba>> 8)&0x000000ff)/0xff, // blue
		(float)((rgba    )&0x000000ff)/0xff  // alpha
	);
}


GLuint createShaderProgram(
	const char *vertPath,  // vertex shader
	const char *tecoPath,  // tessellation control shader
	const char *teevPath,  // tessellation evaluation shader
	const char *geomPath,  // geometry shader
	const char *fragPath,  // fragment shader
	const char *progName
) {
	if (!vertPath) {
		printf("error: no path for mandatory vertex shader\n");
		return 0;
	}
	int textBufSize = 1024;
	#define stageCount 5
	const char *paths[stageCount] = {
		vertPath, tecoPath, teevPath, geomPath, fragPath
	};
	GLenum shaderTypes[stageCount] = {
		GL_VERTEX_SHADER,
		GL_TESS_CONTROL_SHADER,
		GL_TESS_EVALUATION_SHADER,
		GL_GEOMETRY_SHADER,
		GL_FRAGMENT_SHADER
	};
	fr (i, stageCount) {
		if (!paths[i]) continue;
		int sourceSize = getFileSize(paths[i]);
		if (!sourceSize) {
			printf("error: could not open file \"%s\"\n", paths[i]);
			return 0;
		}
		if (sourceSize > textBufSize) textBufSize = sourceSize + 1;
	}
	char *textBuf = malloc(textBufSize);
	GLuint shaderProgram = glCreateProgram();
	GLint success = 0;
	fr (i, stageCount) {
		if (!paths[i]) continue;
		GLuint shader = glCreateShader(shaderTypes[i]);
		stringFromFile(paths[i], textBuf, textBufSize);
		glShaderSource(shader, 1, (const GLchar * const*)&textBuf, NULL);
		glCompileShader(shader);
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, textBufSize, NULL, textBuf);
			printf("error compiling shader \"%s\":\n%s\n", paths[i], textBuf);
			return 0;
		}
		glAttachShader(shaderProgram, shader);
	}
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shaderProgram, textBufSize, NULL, textBuf);
		printf("error linking shader program \"%s\":\n%s\n", progName, textBuf);
		return 0;
	}
	glValidateProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shaderProgram, textBufSize, NULL, textBuf);
		printf("error: invalid shader program \"%s\":\n%s\n", progName, textBuf);
		return 0;
	}
	
	free(textBuf);
	return shaderProgram;
}

#include <SDL2/SDL_surface.h>

void texFromBmp(GLuint tex, const char *bmpPath) {
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	SDL_Surface *srfcRaw = SDL_LoadBMP(bmpPath);_sdlec
	SDL_Surface *srfc  = SDL_ConvertSurfaceFormat(
		srfcRaw, SDL_PIXELFORMAT_ABGR8888, 0
	);_sdlec
	SDL_FreeSurface(srfcRaw);_sdlec
	glTexImage2D(
		GL_TEXTURE_2D,     // GLenum        target
		0,                 // GLint         level
		GL_RGBA,           // GLint         internalformat
		srfc->w,           // GLsizei       width
		srfc->h,           // GLsizei       height
		0,                 // GLint         border
		GL_RGBA,           // GLenum        format
		GL_UNSIGNED_BYTE,  // GLenum        type
		srfc->pixels       // const GLvoid *data
	);
	SDL_FreeSurface(srfc);_sdlec
}

#include <png.h>

void texFromPng(GLuint tex, const char *pngPath, bool mirrorY) {
	FILE *fp = fopen(pngPath, "r");
	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png) abort();
	png_infop info = png_create_info_struct(png);
	if(!info) abort();
	if(setjmp(png_jmpbuf(png))) abort();
	png_init_io(png, fp);
	png_read_info(png, info);
	
	int      width      = png_get_image_width(png, info);
	int      height     = png_get_image_height(png, info);
	png_byte color_type = png_get_color_type(png, info);
	png_byte bit_depth  = png_get_bit_depth(png, info);
	// Read any color_type into 8bit depth, RGBA format.
	// See http://www.libpng.org/pub/png/libpng-manual.txt
	if(bit_depth == 16) png_set_strip_16(png);
	if(color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png);
	// PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
	if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
		png_set_expand_gray_1_2_4_to_8(png);
	}
	if(png_get_valid(png, info, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png);
	// These color_type don't have an alpha channel then fill it with 0xff.
	if(
		color_type == PNG_COLOR_TYPE_RGB  ||
		color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_PALETTE
	) {
		png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
	}
	if(
		color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_GRAY_ALPHA
	) {
		png_set_gray_to_rgb(png);
	}
	png_read_update_info(png, info);
	
	png_byte  *imageData   = malloc(width*4*height);
	png_byte **rowPointers = malloc(sizeof(png_bytep) * height);
	if (mirrorY) {
		fr (i, height) rowPointers[i] = &imageData[(height-i-1)*width*4];
	} else {
		fr (i, height) rowPointers[i] = &imageData[i*width*4];
	}
	png_read_image(png, rowPointers);
	png_destroy_read_struct(&png, &info, NULL);
	free(rowPointers);
	fclose(fp);
	
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(
		GL_TEXTURE_2D,     // GLenum        target
		0,                 // GLint         level
		GL_RGBA,           // GLint         internalformat
		width,             // GLsizei       width
		height,            // GLsizei       height
		0,                 // GLint         border
		GL_RGBA,           // GLenum        format
		GL_UNSIGNED_BYTE,  // GLenum        type
		imageData          // const GLvoid *data
	);
	free(imageData);
}
