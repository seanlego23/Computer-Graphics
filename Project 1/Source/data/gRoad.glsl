#version 410 core
layout (lines_adjacency) in;
layout (triangle_strip, max_vertices = 6) out;

#define PI 3.1415926535897932384626433832795

uniform float curvePrecision;

uniform mat4 m;
uniform mat4 v;
uniform mat4 p;

out vec2 TexCoords;

mat4 rotationMatrix(vec3 axis, float angle) {
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

void main() {
	
	vec3 p0 = vec3(gl_in[0].gl_Position);
	vec3 p1 = vec3(gl_in[1].gl_Position);
	vec3 p2 = vec3(gl_in[2].gl_Position);
	vec3 p3 = vec3(gl_in[3].gl_Position);
	vec4 lineVector = vec4(normalize(p2 - p1), 0.0);
	vec4 nextLineVector = vec4(normalize(p3-p2), 0.0);
	vec3 zero = vec3(0,0,0);
	
	vec3 normal1, normal2;
	if (p0 == zero) {
		normal1 = abs(normalize(cross(vec3(lineVector), p1)));
	} else if (p1 == zero) {
		normal1 = abs(normalize(cross(vec3(lineVector), p0)));
	} else {
		normal1 = abs(normalize(cross(p1, p0)));
	}
	
	if (p2 == zero) {
		normal2 = abs(normalize(cross(vec3(nextLineVector), p3)));
	} else if (p3 == zero) {
		normal2 = abs(normalize(cross(vec3(nextLineVector), p2)));
	} else {
		normal2 = abs(normalize(cross(p3, p2)));
	}
	vec3 side1 = normalize(vec3(rotationMatrix(normal1, PI/2) * lineVector));
	vec3 side2 = normalize(vec3(rotationMatrix(normal2, PI/2) * nextLineVector));
	
	float offset = 1.0;
	float texturePrecision = 0.125;
	mat4 mvp = p * v * m;
	
	gl_Position = mvp * vec4((p1 + (side1 * offset)), 1.0);
	TexCoords = vec2(1.0, gl_PrimitiveIDIn * texturePrecision);
	EmitVertex();
	
	gl_Position = mvp * vec4((p2 + (side2 * offset)), 1.0);
	TexCoords = vec2(1.0, (gl_PrimitiveIDIn + 1) * texturePrecision);
	EmitVertex();
	
	gl_Position = mvp * vec4((p1 - (side1 * offset)), 1.0);
	TexCoords = vec2(0.0, gl_PrimitiveIDIn * texturePrecision);
	EmitVertex();
	
	EndPrimitive();
	
	gl_Position = mvp * vec4((p2 + (side2 * offset)), 1.0);
	TexCoords = vec2(1.0, (gl_PrimitiveIDIn + 1) * texturePrecision);
	EmitVertex();
		
	gl_Position = mvp * vec4((p2 - (side2 * offset)), 1.0);
	TexCoords = vec2(0.0, (gl_PrimitiveIDIn + 1) * texturePrecision);
	EmitVertex();
	
	gl_Position = mvp * vec4((p1 - (side1 * offset)), 1.0);
	TexCoords = vec2(0.0, gl_PrimitiveIDIn * texturePrecision);
	EmitVertex();
	
	EndPrimitive();
}