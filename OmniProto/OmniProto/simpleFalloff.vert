#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

vec3 projectilePos = vec3(0,1.5,0);
vec3 projectileDir = vec3(0,-1,0); //normalized hopefully

const float roughness = 3;
const float radius = 3;
uniform float time;
float falloff(float dist);

void main()
{
    TexCoords = aTexCoords;
	//vec4 projectPos = projection * view * model * vec4(projectilePos, 1.0f);
    vec4 pos = model * vec4(aPos, 1.0);
	pos += vec4(projectileDir * falloff(distance(projectilePos.xyz, pos.xyz)) * ((sin(time) + 1) / 2), 0.0f);
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