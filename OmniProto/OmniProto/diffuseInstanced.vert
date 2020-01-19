#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 instanceMatrix;


out VS_OUT{
	vec2 TexCoords;
	vec3 Normal;
	vec3 FragPos;
} vs_out;

layout (std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

void main()
{
    vs_out.TexCoords = aTexCoords;    
	vs_out.Normal = mat3(transpose(inverse(instanceMatrix))) * aNormal;  
	vs_out.FragPos = vec3(instanceMatrix * vec4(aPos, 1.0));
    gl_Position = projection * view * instanceMatrix * vec4(aPos, 1.0);
}