#version 330
uniform sampler2D texAtl;
in  vec2 texCoordFromGeom;
in  vec4 mulColorFromGeom;
out vec4 outColor;
void main() { outColor = mulColorFromGeom * texture(texAtl, texCoordFromGeom); }
