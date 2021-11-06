#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 Normal;
out vec2 TexCoords;

uniform mat4 m;
uniform mat4 v;
uniform mat4 p;

void main() {
	gl_Position = p*v*m*vec4(aPos, 1.0);
	Normal = 
}