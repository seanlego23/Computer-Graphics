#version 410 core

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
	sampler2D texture;
	sampler2D height;
};

in Material m;
in vec2 TexCoords;

out vec4 FragColor;

void main()
{
   FragColor = texture(m.texture, TexCoords);
}