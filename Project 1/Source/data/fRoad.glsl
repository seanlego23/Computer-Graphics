#version 410 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
	sampler2D texture;
};

uniform Material material;

in vec2 TexCoords;
out vec4 FragColor;

void main()
{
   FragColor = texture(material.texture, TexCoords);
}