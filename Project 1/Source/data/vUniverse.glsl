#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 m;
uniform mat4 v;
uniform mat4 p;

out VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
} vs_out;

void main() {
	
	vs_out.FragPos = (m * vec4(aPos, 1.0)).xyz;
	vs_out.Normal = normalize(mat3(transpose(inverse(m))) * normalize(aNormal));
	vs_out.TexCoords = aTexCoord;

	gl_Position = p*v*m*vec4(aPos, 1.0);
}