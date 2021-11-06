#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec4 color;

uniform mat4 m;
uniform mat4 v;
uniform mat4 p;

uniform double elapsedTime;

void main() {
	vec4 pos = m * vec4(aPos,1.0);
	pos += vec4(0.0, 2.0 - cos(float(elapsedTime)), sin(float(elapsedTime))/1.2, 0.0);
	gl_Position = p * v * pos;
	color = vec4(aPos, 1.0);
}