#version 330

layout(points) in;
layout(triangle_strip) out;
layout(max_vertices = 4) out;

uniform vec2 texAtlSize;
uniform vec2 glyphTexSize;
uniform vec2 scale;

in VS_OUT {
	vec2 texCoord;
} gs_in[];
out vec2 texCoordFromGeom;

void main() {
	texCoordFromGeom = gs_in[0].texCoord / texAtlSize;
	gl_Position = gl_in[0].gl_Position * vec4(scale, 1.0, 1.0);
	EmitVertex();
	texCoordFromGeom = (gs_in[0].texCoord + vec2(glyphTexSize.x, 0.0)) / texAtlSize;
	gl_Position = (gl_in[0].gl_Position + vec4(glyphTexSize.x, 0.0, 0.0, 0.0)) * vec4(scale, 1.0, 1.0);
	EmitVertex();
	texCoordFromGeom = (gs_in[0].texCoord + vec2(0.0, glyphTexSize.y)) / texAtlSize;
	gl_Position = (gl_in[0].gl_Position + vec4(0.0, -glyphTexSize.y, 0.0, 0.0)) * vec4(scale, 1.0, 1.0);
	EmitVertex();
	texCoordFromGeom = (gs_in[0].texCoord + vec2(glyphTexSize.x, glyphTexSize.y)) / texAtlSize;
	gl_Position = (gl_in[0].gl_Position + vec4(glyphTexSize.x, -glyphTexSize.y, 0.0, 0.0)) * vec4(scale, 1.0, 1.0);
	EmitVertex();
	EndPrimitive();
}
