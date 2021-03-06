#version 330

layout(points) in;
layout(triangle_strip) out;
layout(max_vertices = 4) out;

uniform vec2 scale;

in VS_OUT {
	vec2  dstHlfSize;
	vec2  srcPosTpLt;
	vec2  srcSize;
	vec4  mulColor;
	float rotate;
} gs_in[];
out vec2 texCoordFromGeom;
out vec4 mulColorFromGeom;

#define tau 6.28318530717958647692528676655900576839433879875

void main() {
	mulColorFromGeom = gs_in[0].mulColor;
	
	float rotateRad = gs_in[0].rotate * tau;
	mat2 rotateZ = mat2(
		cos(rotateRad),  sin(rotateRad), // left column
		-sin(rotateRad), cos(rotateRad) // right column
	);
	
	texCoordFromGeom = gs_in[0].srcPosTpLt;
	gl_Position = gl_in[0].gl_Position + vec4(scale*(rotateZ*vec2(-gs_in[0].dstHlfSize.x,  gs_in[0].dstHlfSize.y)), 0.0, 0.0);
	EmitVertex();
	
	texCoordFromGeom = gs_in[0].srcPosTpLt + vec2(gs_in[0].srcSize.x, 0.0);
	gl_Position = gl_in[0].gl_Position + vec4(scale*(rotateZ*vec2( gs_in[0].dstHlfSize.x,  gs_in[0].dstHlfSize.y)), 0.0, 0.0);
	EmitVertex();
	
	texCoordFromGeom = gs_in[0].srcPosTpLt + vec2(0.0, gs_in[0].srcSize.y);
	gl_Position = gl_in[0].gl_Position + vec4(scale*(rotateZ*vec2(-gs_in[0].dstHlfSize.x, -gs_in[0].dstHlfSize.y)), 0.0, 0.0);
	EmitVertex();
	
	texCoordFromGeom = gs_in[0].srcPosTpLt + vec2(gs_in[0].srcSize.x, gs_in[0].srcSize.y);
	gl_Position = gl_in[0].gl_Position + vec4(scale*(rotateZ*vec2( gs_in[0].dstHlfSize.x, -gs_in[0].dstHlfSize.y)), 0.0, 0.0);
	EmitVertex();
	
	EndPrimitive();
}
