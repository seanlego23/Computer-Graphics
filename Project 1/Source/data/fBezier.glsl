#version 410 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
	sampler2D texture;
};

uniform Material material;

out vec4 FragColor;

void main()
{
   FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}