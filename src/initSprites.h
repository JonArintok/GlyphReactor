
#include "sprite.h"

extern char *const txtPath;
extern const int   railLength;
extern int         visCharBeg;
extern int         charsSize;
extern char       *chars;
extern sprite     *charSprites;
//extern int         charCount;
extern int         visCharEnd;
extern const int   beamCharPerWidth;
extern int         beamSpritesSize;
extern sprite     *beamSprites;

#define visCharCount_ (visCharEnd-visCharBeg)
#define txtOriginX_ (-railLength*texAtlGlyphW - 64)
#define txtOriginY_ (videoHH_/4)

void initSprites(void);
