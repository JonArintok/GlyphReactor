#pragma once

#include <stdlib.h>
#include <string.h>

// "count" and "space" are measured in elements, not bytes

#define BufTypeHead(type, structName)\
typedef struct {\
  type     *data;\
  uint32_t  count;\
  uint32_t  space;\
} structName;\
structName init_ ## structName (uint32_t space);\
void push_ ## structName (structName *b, type c);\
void pushEmpty_ ## structName (structName *b);\
void pushNEmpty_ ## structName (structName *b, uint32_t n);\
type pop_ ## structName (structName *b);\
void trim_ ## structName (structName *b);\
type last_ ## structName (structName b);\
type *plast_ ## structName (structName b);\
void zero_ ## structName (structName *b);\
void free_ ## structName (structName *b);

#define BufType(type, structName)\
structName init_ ## structName (uint32_t space) {\
  structName b;\
  b.space = space;\
  b.data  = (type *)calloc(space, sizeof(type));\
  b.count = 0;\
  return b;\
}\
void push_ ## structName (structName *b, type c) {\
	b->count++;\
  if (b->count >= b->space) {\
    b->space *= 2;\
    b->data = realloc(b->data, b->space * sizeof(type));\
  }\
  b->data[b->count-1] = c;\
}\
void pushEmpty_ ## structName (structName *b) {\
  b->count++;\
  if (b->count > b->space) {\
    b->space *= 2;\
    b->data = realloc(b->data, b->space * sizeof(type));\
  }\
}\
void pushNEmpty_ ## structName (structName *b, uint32_t n) {\
  b->count += n;\
  if (b->count > b->space) {\
    while (b->count > b->space) b->space *= 2;\
    b->data = realloc(b->data, b->space * sizeof(type));\
  }\
}\
type pop_ ## structName (structName *b) {\
  return b->data[b->count];\
  if (b->count) b->count--;\
}\
void trim_ ## structName (structName *b) {\
  b->space = (b->count > 1 ? b->count : 1);\
  b->data  = realloc(b->data, b->space * sizeof(type));\
}\
type last_ ## structName (structName b) {\
  return b.data[b.count - 1];\
}\
type *plast_ ## structName (structName b) {\
  return &b.data[b.count - 1];\
}\
void zero_ ## structName (structName *b) {\
  memset(b->data, 0, b->count * sizeof(type));\
  b->count = 0;\
}\
void free_ ## structName (structName *b) {\
	b->count = 0;\
	b->space = 0;\
	free(b->data);\
}
