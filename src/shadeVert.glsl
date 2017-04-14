#version 330

uniform vec2 translate;
uniform vec2 scale;
uniform vec2 texAtlSize;

in  vec3 pos;
in  vec2 texCoord;

out VS_OUT {
	vec2 texCoord;
} vs_out;

void main() {
  vs_out.texCoord = texCoord/texAtlSize;
	gl_Position = vec4((pos+vec3(translate, 0.0))*vec3(scale, 1.0), 1.0);
}
