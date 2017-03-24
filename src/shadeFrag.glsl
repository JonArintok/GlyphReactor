#version 330

uniform sampler2D texAtl;
uniform vec2      texAtlSize;

in  vec2 texCoordThru;
out vec4 outColor;

void main() {
  outColor = texture(texAtl, texCoordThru/texAtlSize);
}
