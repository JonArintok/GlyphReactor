#version 330

uniform mat4 transform;

in  vec3 pos;
in  vec2 texCoord;
out vec2 texCoordThru;

void main() {
  texCoordThru = texCoord;
  gl_Position = transform * vec4(pos, 1.0);
}
