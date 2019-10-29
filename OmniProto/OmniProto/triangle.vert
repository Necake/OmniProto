#version 330 core
layout(location = 0) in vec3 aPos;
uniform float sizeX;
out float colorRed;
void main()
{
	colorRed = sizeX;
	gl_Position = vec4(aPos.x * sizeX, aPos.y, aPos.z, 1.0);
}