#version 330

uniform vec2 translate;
uniform vec2 scale;
uniform vec2 texAtlSize;

in vec2  dstPosCntr;
in vec2  dstHlfSize;
in vec2  srcPosTpLt;
in vec2  srcSize;
in vec4  mulColor;
in float rotate;

out VS_OUT {
	vec2  dstHlfSize;
	vec2  srcPosTpLt;
	vec2  srcSize;
	vec4  mulColor;
	float rotate;
} vs_out;

void main() {
	gl_Position       = vec4((dstPosCntr+translate)*scale, 0.0, 1.0);
	vs_out.dstHlfSize = dstHlfSize;
	vs_out.srcPosTpLt = srcPosTpLt/texAtlSize;
	vs_out.srcSize    = srcSize/texAtlSize;
	vs_out.mulColor   = mulColor;
	vs_out.rotate     = rotate;
}
