#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;
in float sinValue;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), (sinValue + 1.0f) / 2.0f) * vec4(0.4f, 0.2f, 0.5f, 1.0f);
}