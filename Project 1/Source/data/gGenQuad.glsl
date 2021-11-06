#version 410 core
layout (points) in;
layout (triangle_strip, max_vertices = 10000) out;

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
	sampler2D texture;
	sampler2D height;
};

uniform Material material;

uniform mat4 m; // model
uniform mat4 v; // view
uniform mat4 p; // perspective
uniform int precision;
uniform vec4 normal;

out vec2 TexCoords;
out Material m;

void main() {
	m = material;

	float startX = gl_in[0].gl_Position.x - 0.5;
	float startY = gl_in[0].gl_Position.y - 0.5;
	float x = startX;
	float y = startY;
	float z = gl_in[0].gl_Position.z;
	float space = 1 / (precision - 1);
	for (int i = 0; i < precision; i++) {
		TexCoords = vec2(x, y - startY);
		gl_Position = p * v * m * (vec4(x, y, z, 1.0) + vec4(normal * (texture(material.height, TexCoords).r / 5.0), 0.0f));
		EmitVertex();
		TexCoords = vec2(x, y - startY + space);
		gl_Position = p * v * m * (vec4(x, y + space, z, 1.0) + vec4(normal * (texture(material.height, TexCoords).r / 5.0), 0.0f));
		EmitVertex();
		x += space;
		for (int j = 1; j < precision; j++) {
			TexCoords = vec2(x - startX, y - startY);
			gl_Position = p * v * m * (vec4(x, y, z, 1.0) + vec4(normal * (texture(material.height, TexCoords).r / 5.0), 0.0f));
			EmitVertex();
			TexCoords = vec2(x - startX, y - startY + space);
			gl_Position = p * v * m * (vec4(x, y + space, z, 1.0) + vec4(normal * (texture(material.height, TexCoords).r / 5.0), 0.0f));
			EmitVertex();
			
		}
		EndPrimitive();
		x = startX;
		y += space;
	}
}