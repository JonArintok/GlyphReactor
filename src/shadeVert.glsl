#version 410

uniform mat4 transform;
uniform vec2 texAtlSize;

in  vec3 pos;
in  vec2 texCoord;
out vec2 texCoordFromVert;

void main() {
  texCoordFromVert = texCoord/texAtlSize;
  gl_Position = transform * vec4(pos, 1.0);
}
