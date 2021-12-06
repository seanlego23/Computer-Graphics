#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 m; // model
uniform mat4 v; // view
uniform mat4 p; // perspective

out vec2 TexCoords;

void main()
{
	gl_Position = p*v*m*vec4(aPos, 1.0);
	TexCoords = aTexCoord;
}