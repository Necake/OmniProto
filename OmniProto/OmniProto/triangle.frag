#version 330 core

out vec4 FragColor;
in float colorRed;
void main()
{
   FragColor = vec4(colorRed, 0.5f, 0.2f, 1.0f);
}