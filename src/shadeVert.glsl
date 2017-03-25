#version 330

uniform vec2 halfVideoSize;

in  vec3 pos;
in  vec2 texCoord;
out vec2 texCoordThru;

void main() {
  texCoordThru = texCoord;
  gl_Position = vec4(pos/vec3(halfVideoSize, 1.0), 1.0);
}
