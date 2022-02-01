#version 410 core
layout (location = 0) out vec4 lColor;

struct Light {				//base alignment	aligned offset
	vec3 position;			//16				0
	vec3 direction;			//16				16
	vec3 color;				//16				32
	vec3 attenuation;		//16				48
	float ambientIntensity;	//4					64
	float diffuseIntensity;	//4					68
	float cutOffDistance;	//4					72
	float cutOffAngle;		//4					76
	float smoothAngle;		//4					80
	int type;				//4					84
};

#define GENERIC_LIGHT_ID		0
#define DIRECTIONAL_LIGHT_ID	1
#define POINT_LIGHT_ID			2
#define SPOT_LIGHT_ID			3

#define MAX_LIGHTS				32

struct Lights {						//base alignment	aligned offset
	int nLights;					//4					0
	Light lights[MAX_LIGHTS];		//96				4				
};

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gMask;

uniform Lights ls;
uniform vec3 cameraPos;
uniform vec2 ScreenSize;
uniform double elapsedTime;
uniform int noLighting;

void main() {
	vec2 TexCoords = gl_FragCoord.xy / ScreenSize;
	vec3 FragPos = texture(gPosition, TexCoords).xyz;
	vec3 Normal = normalize(texture(gNormal, TexCoords).xyz);
	vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;
	float shine = texture(gAlbedoSpec, TexCoords).a;
	
	float mask = texture(gMask, TexCoords).r;

	vec3 color = vec3(0.0, 0.0, 0.0);
	if (mask != 0.0) {
		for (int i = 0; i < ls.nLights; i++) {
			Light light = ls.lights[i];
			
			vec3 ambient = light.color * light.ambientIntensity;
			vec3 diffuse = vec3(0.0, 0.0, 0.0);
			vec3 specular = vec3(0.0, 0.0, 0.0);
			if (light.type == DIRECTIONAL_LIGHT_ID) {
				vec3 lightDir = normalize(-light.direction);
				float diff = max(dot(Normal, lightDir), 0.0);
				
				if (diff > 0.0) {
					diffuse = light.color * diff * Albedo;
					
					vec3 viewDir = normalize(cameraPos - FragPos);
					vec3 halfwayDir = normalize(lightDir + viewDir);
					float spec = pow(max(dot(Normal, halfwayDir), 0.0), shine);
					specular = light.color * spec;
				}
			} else if (light.type == POINT_LIGHT_ID || light.type == SPOT_LIGHT_ID) {
				float cod = light.cutOffDistance;
				if (cod < 0) { //Calculate when attenuation function goes below .01
					vec3 att = light.attenuation;
					cod = (-att.y + sqrt(pow(att.y, 2) - 4 * att.z * (att.x - 100))) / (2 * att.z); //quadratic formula
				}
				
				float dist = length(FragPos - light.position);
				if (dist < cod) {
					float attFactor = 1.0 / (light.attenuation.x + light.attenuation.y * dist + light.attenuation.z * pow(dist, 2));
					
					vec3 lightDir = normalize(light.position - FragPos);
					float diff = max(dot(Normal, lightDir), 0.0);
					diffuse = light.color * diff * Albedo * attFactor;
					
					vec3 viewDir = normalize(cameraPos - FragPos);
					vec3 halfwayDir = normalize(lightDir + viewDir);
					float spec = pow(max(dot(Normal, halfwayDir), 0.0), shine);
					specular = light.color * spec * attFactor;
					
					if (light.type == SPOT_LIGHT_ID) { //SPOT_LIGHT_ID
						float theta = dot(lightDir, normalize(-light.direction));
						float epsilon = light.cutOffAngle - light.smoothAngle;
						float intensity = (epsilon != 0 ? clamp((theta - light.smoothAngle) / epsilon, 0.0, 1.0) : 0.0);
						
						diffuse *= intensity;
						specular *= intensity;
					}
				}
			}
			color += ambient + diffuse + specular;
		}
	} else {
		color = Albedo;
	}
	lColor = vec4(color, 1.0);
}