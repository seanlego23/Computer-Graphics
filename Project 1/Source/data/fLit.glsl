#version 410 core

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
	sampler2D texture;
	int useTexture;
};

struct Light {
	vec3 position;
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 attenuation;
	float intensity;
	float cutOffAngle;
	float smoothAngle;
	int type;
};

#define GENERIC_LIGHT_ID		0
#define DIRECTIONAL_LIGHT_ID	1
#define POINT_LIGHT_ID			2
#define SPOT_LIGHT_ID			3

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform Material material;
uniform Light light;
uniform vec3 cameraPos;
uniform double elapsedTime;

out vec4 FragColor;

void main() {
	vec4 texColor;
	if (material.useTexture == 1) {
		texColor = texture(material.texture, fs_in.TexCoords);
	}
	
	vec3 lightDir;
	if (light.type == DIRECTIONAL_LIGHT_ID) {
		lightDir = normalize(-light.direction);
	} else {
		lightDir = normalize(light.position - fs_in.FragPos);
	}
	
	//ambient
	vec3 ambient = light.ambient * vec3(material.ambient);
	
	//diffuse
	float diff = max(dot(fs_in.Normal, lightDir), 0.0);
	vec3 diffuse = diff * light.diffuse * vec3(material.diffuse);
	
	//specular
	vec3 viewDir = normalize(cameraPos - fs_in.FragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(fs_in.Normal, halfwayDir), 0.0), material.shininess);
	vec3 specular = spec * light.specular * vec3(material.specular);
	
	vec3 lightColor = ambient + diffuse + specular;
	
	if (material.useTexture == 1) {
		FragColor = 0.8 * texColor + 0.2 * vec4(lightColor, 1.0);
	} else {
		FragColor = vec4(lightColor, 1.0);
	}
}