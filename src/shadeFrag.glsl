#version 330
uniform sampler2D texAtl;
in  vec2 texCoordFromGeom;
out vec4 outColor;
void main() {outColor = texture(texAtl, texCoordFromGeom);}
