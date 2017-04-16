// Origin is in top left corner

#define texAtlPath "img/texAtlas.png"

#define texAtlW 256
#define texAtlH 256
#define texAtlGlyphW 16
#define texAtlGlyphH 37
#define texAtlGlyphsPerRow 16
#define texAtlGlyphsRowCount 6
#define texAtlGlyphsW (texAtlGlyphW*texAtlGlyphsPerRow)
#define texAtlGlyphsH (texAtlGlyphH*texAtlGlyphsRowCount)
#define texAtlGlyphsStartX 0
#define texAtlGlyphsStartY 0
#define texAtlGlyphsEndX (texAtlGlyphsStartX+texAtlGlyphsW)
#define texAtlGlyphsEndY (texAtlGlyphsStartY+texAtlGlyphsH)
#define texAtlGlyphsAsciiStart 32
#define bkspChar 127

// texel coordinates of top left corner of glyph
#define texAtlGlyphPosX(glyphVal) \
  (((glyphVal-texAtlGlyphsAsciiStart)%texAtlGlyphsPerRow)*texAtlGlyphW)
// end macro
#define texAtlGlyphPosY(glyphVal) \
  (((glyphVal-texAtlGlyphsAsciiStart)/texAtlGlyphsPerRow)*texAtlGlyphH)
// end macro
