#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 normal;
    mat4 model;
    vec3 pos;
} gs_in[];

layout (std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

const float MAGNITUDE = 0.4f;

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
    EmitVertex();
    vec3 norm = normalize(cross(vec3(gl_in[1].gl_Position - gl_in[0].gl_Position),vec3(gl_in[2].gl_Position - gl_in[0].gl_Position)));
    gl_Position = gl_in[0].gl_Position - projection * view * gs_in[0].model * vec4(normalize(norm), 1) * MAGNITUDE;
    EmitVertex();
    EndPrimitive();
    
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    norm = normalize(cross(vec3(gl_in[2].gl_Position - gl_in[1].gl_Position),vec3(gl_in[0].gl_Position - gl_in[1].gl_Position)));
    gl_Position = gl_in[1].gl_Position - projection * view * gs_in[1].model * vec4(normalize(norm), 1) * MAGNITUDE;
    EmitVertex();
    EndPrimitive();

    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
    norm = normalize(cross(vec3(gl_in[0].gl_Position - gl_in[2].gl_Position),vec3(gl_in[1].gl_Position - gl_in[2].gl_Position)));
    gl_Position = gl_in[2].gl_Position - projection * view * gs_in[2].model * vec4(normalize(norm), 1) * MAGNITUDE;
    EmitVertex();
    EndPrimitive();
}