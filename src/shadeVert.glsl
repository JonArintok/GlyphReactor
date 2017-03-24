#version 330

uniform vec2 videoSize;

in  vec3 pos;
in  vec2 texCoord;
out vec2 texCoordThru;

void main() {
  texCoordThru = texCoord;
  gl_Position = vec4((pos*2)/vec3(videoSize, 1.0), 1.0);
}
