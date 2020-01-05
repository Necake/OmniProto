#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT
{
	vec2 texCoords;
	vec3 normal;
    vec3 FragPos;
    mat4 model;
} gs_in[];

layout (std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

out GS_OUT{
	vec2 TexCoords;
	vec3 Normal;
	vec3 FragPos;
} gs_out;

out vec3 norm;

const float MAGNITUDE = 0.2f;

void GenerateLine(int index)
{
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = gl_in[index].gl_Position + projection * view * vec4(normalize(gs_in[index].normal), 0.0) * MAGNITUDE;
    EmitVertex();
    EndPrimitive();
}

void main()
{
    gl_Position = gl_in[0].gl_Position;
    gs_out.TexCoords = gs_in[0].texCoords;
    vec3 norm = normalize(cross(vec3(gl_in[1].gl_Position - gl_in[0].gl_Position),vec3(gl_in[2].gl_Position - gl_in[0].gl_Position)));
    gs_out.Normal = -norm;
    gs_out.FragPos = gs_in[0].FragPos;
    EmitVertex();
    //gl_Position = gl_in[0].gl_Position - vec4(normalize(norm), 0) * MAGNITUDE;
    //EmitVertex();
    
    gl_Position = gl_in[1].gl_Position;
    gs_out.TexCoords = gs_in[1].texCoords;
    norm = normalize(cross(vec3(gl_in[2].gl_Position - gl_in[1].gl_Position),vec3(gl_in[0].gl_Position - gl_in[1].gl_Position)));
    gs_out.Normal = -norm;
    gs_out.FragPos = gs_in[1].FragPos;
    EmitVertex();
    //gl_Position = gl_in[1].gl_Position - vec4(normalize(norm), 0) * MAGNITUDE;
    //EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    gs_out.TexCoords = gs_in[2].texCoords;
    norm = normalize(cross(vec3(gl_in[0].gl_Position - gl_in[2].gl_Position),vec3(gl_in[1].gl_Position - gl_in[2].gl_Position)));
    gs_out.Normal = -norm;
    gs_out.FragPos = gs_in[2].FragPos;
    EmitVertex();
    //gl_Position = gl_in[2].gl_Position - vec4(normalize(norm), 0) * MAGNITUDE;
    //EmitVertex();
    EndPrimitive();
}