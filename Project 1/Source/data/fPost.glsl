#version 410 core

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
	sampler2D texture;
	int useTexture;
};

in vec2 TexCoords;

uniform Material material;
uniform vec2 ScreenSize;
uniform float exposure;

out vec4 FragColor;

void main() {
	const float gamma = 0.3;
	vec4 texColor = texture(material.texture, TexCoords);
	vec3 hdrColor = texColor.rgb;
	
	vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
	
	mapped = pow(mapped, vec3(1.0 / gamma));
	
	FragColor = vec4(mapped, texColor.a);
}