#version 410 core

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

out vec4 FragColor;

void main() {
	FragColor = vec4(texture(gAlbedoSpec, TexCoords).rgb, 1.0);
}