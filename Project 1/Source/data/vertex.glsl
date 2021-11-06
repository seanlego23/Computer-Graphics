#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 m; // model
uniform mat4 v; // view
uniform mat4 p; // perspective

uniform int instances;

void main()
{
	int side = int(sqrt(instances));
	float x = aPos.x - side / 2 + mod(gl_InstanceID, side);
	float y = aPos.y - side / 2 + int(gl_InstanceID / side);
	gl_Position = p * v * m * vec4(x, y, aPos.z, 1.0);
	
	TexCoord = vec2(aTexCoord.x / 3.0, aTexCoord.y / 3.0);
}