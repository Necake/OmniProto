//Pass-through geometry shader

#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT{
	vec2 TexCoords;
	vec3 Normal;
	vec3 FragPos;
    vec4 FragPosLightSpace;
} gs_in[];

out GS_OUT{
	vec2 TexCoords;
	vec3 Normal;
	vec3 FragPos;
    vec4 FragPosLightSpace;
} gs_out;

void main()
{
	gl_Position = gl_in[0].gl_Position;
	gs_out.TexCoords = gs_in[0].TexCoords;
	gs_out.Normal = gs_in[0].Normal;
	gs_out.FragPos = gs_in[0].FragPos;
	gs_out.FragPosLightSpace = gs_in[0].FragPosLightSpace;
	EmitVertex();
	gl_Position = gl_in[1].gl_Position;
	gs_out.TexCoords = gs_in[1].TexCoords;
	gs_out.Normal = gs_in[1].Normal;
	gs_out.FragPos = gs_in[1].FragPos;
	gs_out.FragPosLightSpace = gs_in[1].FragPosLightSpace;
	EmitVertex();
	gl_Position = gl_in[2].gl_Position;
	gs_out.TexCoords = gs_in[2].TexCoords;
	gs_out.Normal = gs_in[2].Normal;
	gs_out.FragPos = gs_in[2].FragPos;
	gs_out.FragPosLightSpace = gs_in[2].FragPosLightSpace;
	EmitVertex();
	EndPrimitive();
}