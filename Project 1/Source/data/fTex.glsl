#version 410 core

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
	sampler2D texture;
	int useTexture;
};

uniform Material material;

in vec2 TexCoords;
out vec4 FragColor;

void main()
{
   FragColor = texture(material.texture, TexCoords);
}