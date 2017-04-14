#version 330

layout(points) in;
layout(triangle_strip) out;
layout(max_vertices = 4) out;

uniform vec2 scale;
uniform vec2 texAtlSize;
uniform vec2 glyphTexSize;

in VS_OUT {
	vec2 texCoord;
} gs_in[];
out vec2 texCoordFromGeom;

void main() {
	vec2 srcSize = glyphTexSize/texAtlSize;
	vec2 dstSize = glyphTexSize*scale;
	
	texCoordFromGeom = gs_in[0].texCoord;
	gl_Position      = gl_in[0].gl_Position;
	EmitVertex();
	
	texCoordFromGeom = gs_in[0].texCoord    + vec2(srcSize.x, 0.0);
	gl_Position      = gl_in[0].gl_Position + vec4(dstSize.x, 0.0, 0.0, 0.0);
	EmitVertex();
	
	texCoordFromGeom = gs_in[0].texCoord    + vec2(0.0,  srcSize.y);
	gl_Position      = gl_in[0].gl_Position + vec4(0.0, -dstSize.y, 0.0, 0.0);
	EmitVertex();
	
	texCoordFromGeom = gs_in[0].texCoord    + vec2(srcSize.x,  srcSize.y);
	gl_Position      = gl_in[0].gl_Position + vec4(dstSize.x, -dstSize.y, 0.0, 0.0);
	EmitVertex();
	
	EndPrimitive();
}
