#version 330

uniform vec2 translate;

in  vec3 pos;
in  vec2 texCoord;

out VS_OUT {
	vec2 texCoord;
} vs_out;

void main() {
  vs_out.texCoord = texCoord;
	gl_Position = vec4(pos+vec3(translate, 0.0), 1.0);
}
