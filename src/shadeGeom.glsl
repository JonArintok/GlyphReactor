#version 410

layout(points) in;
layout(triangle_strip) out;
layout(max_vertices = 4) out;

uniform vec2 glyphTexSize;
uniform vec2 glyphSpaSize;
in  vec2 texCoordFromVert[];
out vec2 texCoordFromGeom;

void main() {
	texCoordFromGeom = texCoordFromVert[0];
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	texCoordFromGeom = texCoordFromVert[0] + vec2(glyphTexSize.x, 0.0);
	gl_Position = gl_in[0].gl_Position + vec4(glyphSpaSize.x, 0.0, 0.0, 0.0);
	EmitVertex();
	texCoordFromGeom = texCoordFromVert[0] + vec2(0.0, glyphTexSize.y);
	gl_Position = gl_in[0].gl_Position + vec4(0.0, -glyphSpaSize.y, 0.0, 0.0);
	EmitVertex();
	texCoordFromGeom = texCoordFromVert[0] + vec2(glyphTexSize.x, glyphTexSize.y);
	gl_Position = gl_in[0].gl_Position + vec4(glyphSpaSize.x, -glyphSpaSize.y, 0.0, 0.0);
	EmitVertex();
	EndPrimitive();
}
