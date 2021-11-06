#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

uniform mat4 m; // model
uniform mat4 v; // view
uniform mat4 p; // perspective

void main()
{
	gl_Position = p*v*m*vec4(aPos, 1.0);
	ourColor = aColor;
}