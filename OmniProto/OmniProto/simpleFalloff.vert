#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT{
	vec2 texCoords;
	vec3 normal;
    vec3 FragPos;
    mat4 model;
} vs_out;

layout (std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};
uniform mat4 model;

uniform vec3 projectilePos;// = vec3(0,1.5,0);
vec3 projectileDir = vec3(0,-1,0); //normalized hopefully

const float roughness = 3;
uniform float radius;// = 3;
uniform float time;
float falloff(float dist);

void main()
{
    vs_out.texCoords = aTexCoords;
	vs_out.normal = mat3(transpose(inverse(model))) * aNormal;
	vs_out.model = model;
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
	//vec4 projectPos = projection * view * model * vec4(projectilePos, 1.0f);
    vec4 pos = model * vec4(aPos, 1.0);
	pos += vec4(projectileDir * falloff(distance(projectilePos.xyz, pos.xyz)), 0.0f);
	pos = projection * view * pos;
	//pos += vec4(1,0,0,0);
	gl_Position = pos;
}

float falloff(float dist)
{
	if(dist > radius)
	{
		return 0;
	}
	float val = 1.0f - pow(dist / radius, 2);
	val = pow(val, roughness);
	return val;
}