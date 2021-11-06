#version 410 core

uniform sampler2D tex;

uniform vec4 ourColor;

in vec2 TexCoord;
out vec4 FragColor;

void main()
{
   FragColor = mix(texture(tex, TexCoord), texture(tex, vec2(3 - TexCoord.x, 3 - TexCoord.y)), .3);
}