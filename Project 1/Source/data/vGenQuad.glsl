#version 410 core

layout (location = 0) in vec3 aPos;

uniform int instances;

void main()
{
	int side = int(sqrt(instances));
	float x = aPos.x - side / 2 + mod(gl_InstanceID, side);
	float y = aPos.y - side / 2 + int(gl_InstanceID / side);
	gl_Position = vec4(x, y, aPos.z, 1.0);
}