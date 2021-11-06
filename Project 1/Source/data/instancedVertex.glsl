layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 m; // model
uniform mat4 v; // view
uniform mat4 p; // perspective

uniform mat4[7] inst; //model instance

void main()
{
	gl_Position = p*v*m*inst[gl_InstanceID]*vec4(aPos, 1.0);
	TexCoord = aTexCoord;
}