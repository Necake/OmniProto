#version 330 core
out vec4 FragColor;

void main()
{
	FragColor = vec4(gl_FragCoord.x / 1920, 1 - gl_FragCoord.y / 1080,1,1);
}