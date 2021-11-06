#version 410 core

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
	sampler2D texture;
};

uniform Material material;

in vec4 color;
out vec4 FragColor;

void main()
{
   FragColor = color;
}