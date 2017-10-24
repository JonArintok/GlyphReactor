// Origin is in top left corner

#define texAtlPath "img/texAtlas.png"

#define texAtlW 512
#define texAtlH 512
#define texAtlGlyphW 16
#define texAtlGlyphH 37
#define texAtlGlyphsPerRow 16
#define texAtlGlyphsRowCount 6
#define texAtlGlyphsCount 96
#define texAtlGlyphsW (texAtlGlyphW*texAtlGlyphsPerRow)
#define texAtlGlyphsH (texAtlGlyphH*texAtlGlyphsRowCount)
#define texAtlGlyphsStartX 0
#define texAtlGlyphsStartY 0
#define texAtlGlyphsEndX (texAtlGlyphsStartX+texAtlGlyphsW)
#define texAtlGlyphsEndY (texAtlGlyphsStartY+texAtlGlyphsH)
#define texAtlGlyphsAsciiStart 32
#define bkspChar 127

#define texAtlGunX  258 // left edge
#define texAtlGunY    2 // top edge
#define texAtlGunW  252
#define texAtlGunH  237
#define texAtlBeltX 325 // left edge
#define texAtlBeltY 239 // top edge
#define texAtlBeltW 118
#define texAtlBeltH   1

#define texAtlMenuCursorX 240 // left edge
#define texAtlMenuCursorY 233 // top edge
#define texAtlMenuCursorW  16
#define texAtlMenuCursorH  16


// texel coordinates of top left corner of glyph
#define texAtlGlyphPosX(glyphVal) \
  (((glyphVal-texAtlGlyphsAsciiStart)%texAtlGlyphsPerRow)*texAtlGlyphW)
// end macro
#define texAtlGlyphPosY(glyphVal) \
  (((glyphVal-texAtlGlyphsAsciiStart)/texAtlGlyphsPerRow)*texAtlGlyphH)
// end macro
