#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

#define GENERIC_LIGHT_ID		0
#define DIRECTIONAL_LIGHT_ID	1
#define POINT_LIGHT_ID			2
#define SPOT_LIGHT_ID			3

class Light {
	const std::string lightName;
	const unsigned short type_id;

	bool listed = true;

protected:
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float lightIntensity;

public:

	static std::vector<Light*> lights;

	Light() = delete;

	Light(const Light& rhs) : lightName(rhs.lightName + " Copy"), type_id(rhs.type_id), 
		ambient(rhs.ambient), diffuse(rhs.diffuse), specular(rhs.specular), lightIntensity(rhs.lightIntensity) { }

	Light(std::string name, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float intensity = 1.0f) : lightName(name), type_id(GENERIC_LIGHT_ID),
		ambient(amb), diffuse(diff), specular(spec), lightIntensity(intensity) { }

	Light(std::string name, unsigned short type, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float intensity = 1.0f)
		: lightName(name), type_id(type), ambient(amb), diffuse(diff), specular(spec), lightIntensity(intensity) { }

	virtual ~Light() { }

	std::string getName() const { return lightName; }

	unsigned short getTypeID() const { return type_id; }

	glm::vec3 getAmbient() const { return ambient; }

	glm::vec3 getDiffuse() const { return diffuse; }

	glm::vec3 getSpecular() const { return specular; }

	float getIntensity() const { return lightIntensity; }

	bool isImGuiListed() const { return listed; }

	void setAmbient(glm::vec3 amb) { ambient = amb; }

	void setDiffuse(glm::vec3 diff) { diffuse = diff; }

	void setSpecular(glm::vec3 spec) { specular = spec; }

	void setIntensity(float intensity) { lightIntensity = intensity; }

	void setImGuiListed(bool list) { listed = list; }

	virtual void use(unsigned int shaderID) = 0;

	virtual void ImGui();
};

class DirectionalLight : public Light {
	static unsigned int num;

protected:
	glm::vec3 lightDir;

public:

	DirectionalLight() = delete;

	DirectionalLight(const DirectionalLight& rhs) : Light(rhs), lightDir(rhs.lightDir) { }

	DirectionalLight(glm::vec3 direction, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec) 
		: Light("DirectionalLight-" + std::to_string(++num), DIRECTIONAL_LIGHT_ID, amb, diff, spec), lightDir(direction) { }

	DirectionalLight(glm::vec3 direction, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float intensity) 
		: Light("DirectionalLight-" + std::to_string(++num), DIRECTIONAL_LIGHT_ID, amb, diff, spec, intensity), lightDir(direction) { }

	virtual ~DirectionalLight() { }

	glm::vec3 getDirection() const { return lightDir; }

	void setDirection(glm::vec3 direction) { lightDir = direction; }

	virtual void use(unsigned int shaderID) {
		glUniform3f(glGetUniformLocation(shaderID, "light.direction"), lightDir.x, lightDir.y, lightDir.z);
		glUniform3f(glGetUniformLocation(shaderID, "light.ambient"), ambient.r, ambient.g, ambient.b);
		glUniform3f(glGetUniformLocation(shaderID, "light.diffuse"), diffuse.r, diffuse.g, diffuse.b);
		glUniform3f(glGetUniformLocation(shaderID, "light.specular"), specular.r, specular.g, specular.b);
		glUniform1f(glGetUniformLocation(shaderID, "light.intensity"), lightIntensity);
		glUniform1i(glGetUniformLocation(shaderID, "light.type"), DIRECTIONAL_LIGHT_ID);
	}

	virtual void ImGui();
};

class PointLight : public Light {
	static unsigned int num;

protected:
	glm::vec3 lightPos;
	glm::vec3 att;

public:

	PointLight() = delete;

	PointLight(const PointLight& rhs) : Light(rhs), lightPos(rhs.lightPos), att(rhs.att) { }

	PointLight(glm::vec3 position, glm::vec3 attenuation, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec)
		: Light("PointLight-" + std::to_string(++num), POINT_LIGHT_ID, amb, diff, spec), lightPos(position), att(attenuation) { }

	virtual ~PointLight() { }

	glm::vec3 getPosition() const { return lightPos; }

	glm::vec3 getAttenuation() const { return att; }

	void setPosition(glm::vec3 position) { lightPos = position; }

	void setAttenuation(glm::vec3 attenuation) { att = attenuation; }

	virtual void use(unsigned int shaderID) {
		glUniform3f(glGetUniformLocation(shaderID, "light.position"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(shaderID, "light.attenuation"), att.s, att.t, att.p);
		glUniform3f(glGetUniformLocation(shaderID, "light.ambient"), ambient.r, ambient.g, ambient.b);
		glUniform3f(glGetUniformLocation(shaderID, "light.diffuse"), diffuse.r, diffuse.g, diffuse.b);
		glUniform3f(glGetUniformLocation(shaderID, "light.specular"), specular.r, specular.g, specular.b);
		glUniform1f(glGetUniformLocation(shaderID, "light.intensity"), lightIntensity);
		glUniform1i(glGetUniformLocation(shaderID, "light.type"), POINT_LIGHT_ID);
	}

	virtual void ImGui();
};

class SpotLight : public Light {
	static unsigned int num;

protected:
	glm::vec3 lightPos;
	glm::vec3 lightDir;

	float cutOffAngle;
	float smoothAngle;

public:

	SpotLight() = delete;

	SpotLight(const SpotLight& rhs) : Light(rhs), lightPos(rhs.lightPos), lightDir(rhs.lightDir), 
		cutOffAngle(rhs.cutOffAngle), smoothAngle(rhs.smoothAngle) { }

	SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float theta)
		: Light("SpotLight-" + std::to_string(++num), SPOT_LIGHT_ID, amb, diff, spec), lightPos(position), lightDir(direction), 
		cutOffAngle(theta), smoothAngle(theta) { }

	SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float theta, float gamma)
		: Light("SpotLight-" + std::to_string(++num), SPOT_LIGHT_ID, amb, diff, spec), lightPos(position), lightDir(direction), 
		cutOffAngle(theta), smoothAngle(gamma) { }

	SpotLight(glm::vec3 position, glm::vec3 direction, float intensity, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float theta)
		: Light("SpotLight-" + std::to_string(++num), SPOT_LIGHT_ID, amb, diff, spec, intensity), lightPos(position), lightDir(direction), 
		cutOffAngle(theta), smoothAngle(theta) { }

	SpotLight(glm::vec3 position, glm::vec3 direction, float intensity, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float theta, float gamma)
		: Light("SpotLight-" + std::to_string(++num), SPOT_LIGHT_ID, amb, diff, spec, intensity), lightPos(position), lightDir(direction), 
		cutOffAngle(theta), smoothAngle(gamma) { }

	virtual ~SpotLight() { }

	glm::vec3 getPosition() const { return lightPos; }

	glm::vec3 getDirection() const { return lightDir; }

	float getCutOffAngle() const { return cutOffAngle; }

	float getSmoothingAngle() const { return smoothAngle; }

	void setPosition(glm::vec3 position) { lightPos = position; }

	void setDirection(glm::vec3 direction) { lightDir = direction; }

	void setCutOffAngle(float phi) { cutOffAngle = phi; }

	void setSmoothingAngle(float gamma) { smoothAngle = gamma; }

	virtual void use(unsigned int shaderID) {
		glUniform3f(glGetUniformLocation(shaderID, "light.position"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(shaderID, "light.direction"), lightDir.x, lightDir.y, lightDir.z);
		glUniform3f(glGetUniformLocation(shaderID, "light.ambient"), ambient.r, ambient.g, ambient.b);
		glUniform3f(glGetUniformLocation(shaderID, "light.diffuse"), diffuse.r, diffuse.g, diffuse.b);
		glUniform3f(glGetUniformLocation(shaderID, "light.specular"), specular.r, specular.g, specular.b);
		glUniform1f(glGetUniformLocation(shaderID, "light.intensity"), lightIntensity);
		glUniform1f(glGetUniformLocation(shaderID, "light.cutOffAngle"), cutOffAngle);
		glUniform1f(glGetUniformLocation(shaderID, "light.smoothAngle"), smoothAngle);
		glUniform1i(glGetUniformLocation(shaderID, "light.type"), SPOT_LIGHT_ID);
	}

	virtual void ImGui();
};
