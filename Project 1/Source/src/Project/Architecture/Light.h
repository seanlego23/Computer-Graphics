#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

#include "Object.h"
#include "shader_s.h"

#define GENERIC_LIGHT_ID		0
#define DIRECTIONAL_LIGHT_ID	1
#define POINT_LIGHT_ID			2
#define SPOT_LIGHT_ID			3

class Light : public Object {
	const unsigned short type_id;

	bool listed = true;

	glm::vec3 lightColor;
	
	float ambientIntensity;
	float diffuseIntensity;

public:

	static std::vector<std::shared_ptr<Light>> lights;

	Light() = delete;

	Light(const Light& rhs) : Object(rhs), type_id(rhs.type_id), lightColor(rhs.lightColor), 
		ambientIntensity(rhs.ambientIntensity), diffuseIntensity(rhs.diffuseIntensity) { 
		lights.push_back(std::shared_ptr<Light>(this));
	}

	Light(std::string name, glm::vec3 color, float ambIntensity = 0.2f, float diffIntensity = 1.0f) : Object(name), type_id(GENERIC_LIGHT_ID),
		lightColor(color), ambientIntensity(ambIntensity), diffuseIntensity(diffIntensity) { 
		lights.push_back(std::shared_ptr<Light>(this));
	}

protected:

	Light(std::string name, unsigned short type, glm::vec3 color, float ambIntensity = 0.2f, float diffIntensity = 1.0f)
		: Object(name), type_id(type), lightColor(color), ambientIntensity(ambIntensity), diffuseIntensity(diffIntensity) { 
		lights.push_back(std::shared_ptr<Light>(this));
	}

public:

	virtual ~Light() { }

	unsigned short getTypeID() const { return type_id; }

	glm::vec3 getColor() const { return lightColor; }

	float getAmbientIntensity() const { return ambientIntensity; }

	float getDiffuseIntensity() const { return diffuseIntensity; }

	bool isImGuiListed() const { return listed; }

	void setColor(glm::vec3 color) {
		this->setDirty();
		lightColor = color;
	}

	void setAmbientIntensity(float ambIntensity) {
		this->setDirty();
		ambientIntensity = ambIntensity;
	}

	void setDiffuseIntensity(float diffIntensity) {
		this->setDirty();
		diffuseIntensity = diffIntensity;
	}

	void setImGuiListed(bool list) { listed = list; }

	virtual void use(unsigned int shaderID, int index) = 0;

	virtual void ImGui();

	virtual void ImGui() const;
};

class DirectionalLight : public Light {
	static unsigned int num;

protected:
	glm::vec3 lightDir;

public:

	DirectionalLight() = delete;

	DirectionalLight(const DirectionalLight& rhs) : Light(rhs), lightDir(rhs.lightDir) { 
		++num;
	}

	DirectionalLight(glm::vec3 direction, glm::vec3 color = glm::vec3(1.0f), float ambIntensity = 0.2f, float diffIntensity = 1.0f) 
		: Light("DirectionalLight-" + std::to_string(++num), DIRECTIONAL_LIGHT_ID, color, ambIntensity, diffIntensity), lightDir(direction) { }

	DirectionalLight(std::string name, glm::vec3 direction, glm::vec3 color = glm::vec3(1.0f), float ambIntensity = 0.2f, float diffIntensity = 1.0f)
		: Light(name, DIRECTIONAL_LIGHT_ID, color, ambIntensity, diffIntensity), lightDir(direction) { 
		++num;
	}

	virtual ~DirectionalLight() { }

	glm::vec3 getDirection() const { return lightDir; }

	void setDirection(glm::vec3 direction) {
		this->setDirty();
		lightDir = direction;
	}

	virtual void use(unsigned int shaderID, int index);

	virtual void ImGui();

	virtual void ImGui() const;
};

class PointLight : public Light {
	static unsigned int num;

protected:
	glm::vec3 lightPos;
	glm::vec3 att;
	float cutOffDistance; //A value below zero represents using the attenuation function when it goes below a certain value (usually 0.01f)

public:

	PointLight() = delete;

	PointLight(const PointLight& rhs) : Light(rhs), lightPos(rhs.lightPos), att(rhs.att), cutOffDistance(rhs.cutOffDistance) { 
		++num;
	}

	PointLight(glm::vec3 position, glm::vec3 attenuation, float maxDistance = -1.0f, 
			   glm::vec3 color = glm::vec3(1.0f), float ambIntensity = 0.2f, float diffIntensity = 1.0f)
		: Light("PointLight-" + std::to_string(++num), POINT_LIGHT_ID, color, ambIntensity, diffIntensity), lightPos(position), att(attenuation),
		cutOffDistance(maxDistance) { }

	PointLight(std::string name, glm::vec3 position, glm::vec3 attenuation, float maxDistance = -1.0f, 
			   glm::vec3 color = glm::vec3(1.0f), float ambIntensity = 0.2f, float diffIntensity = 1.0f)
		: Light(name, POINT_LIGHT_ID, color, ambIntensity, diffIntensity), lightPos(position), att(attenuation), cutOffDistance(maxDistance) {
		++num;
	}

protected:

	PointLight(std::string name, unsigned short id, glm::vec3 position, glm::vec3 attenuation, 
			   float maxDistance = -1.0f, glm::vec3 color = glm::vec3(1.0f), float ambIntensity = 0.2f,
			   float diffIntensity = 1.0f) : Light(name, id, color, ambIntensity, diffIntensity), lightPos(position), att(attenuation),
		cutOffDistance(maxDistance) { }

public:

	virtual ~PointLight() { }

	glm::vec3 getPosition() const { return lightPos; }

	glm::vec3 getAttenuation() const { return att; }

	float getMaxDistance() const { return cutOffDistance; }

	void setPosition(glm::vec3 position) {
		this->isDirty();
		lightPos = position;
	}

	void setAttenuation(glm::vec3 attenuation) {
		this->isDirty();
		att = attenuation;
	}

	void setConstantAttenuation(float constant) {
		this->isDirty();
		att.x = constant;
	}

	void setLinearAttenuation(float linear) {
		this->isDirty();
		att.y = linear;
	}

	void setQuadraticAttenuation(float quadratic) {
		this->isDirty();
		att.z = quadratic;
	}

	void setMaxDistance(float maxDistance) {
		this->isDirty();
		cutOffDistance = maxDistance;
	}

	virtual void use(unsigned int shaderID, int index);

	virtual void ImGui();

	virtual void ImGui() const;
};

class SpotLight : public PointLight {
	static unsigned int num;

protected:
	glm::vec3 lightDir;

	float cutOffAngle;
	float smoothAngle;

public:

	SpotLight() = delete;

	SpotLight(const SpotLight& rhs) : PointLight(rhs), lightDir(rhs.lightDir), cutOffAngle(rhs.cutOffAngle), smoothAngle(rhs.smoothAngle) { 
		++num;
	}

	SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 attenuation, float theta, float maxDistance = -1.0f, 
			  glm::vec3 color = glm::vec3(1.0f), float ambIntensity = 0.2f, float diffIntensity = 1.0f)
		: PointLight("SpotLight-" + std::to_string(++num), SPOT_LIGHT_ID, position, attenuation, maxDistance, color, ambIntensity, diffIntensity), 
		lightDir(direction), cutOffAngle(theta), smoothAngle(theta) { }

	SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 attenuation, float theta, float gamma, 
			  float maxDistance = -1.0f, glm::vec3 color = glm::vec3(1.0f), float ambIntensity = 0.2f,
			  float diffIntensity = 1.0f)
		: PointLight("SpotLight-" + std::to_string(++num), SPOT_LIGHT_ID, position, attenuation, maxDistance, color, ambIntensity, diffIntensity),
		lightDir(direction), cutOffAngle(theta), smoothAngle(gamma) {
		assert(gamma >= theta);
	}

	SpotLight(std::string name, glm::vec3 position, glm::vec3 direction, glm::vec3 attenuation, float theta,
			  float maxDistance = -1.0f, glm::vec3 color = glm::vec3(1.0f), float ambIntensity = 0.2f,
			  float diffIntensity = 1.0f)
		: PointLight(name, SPOT_LIGHT_ID, position, attenuation, maxDistance, color, ambIntensity, diffIntensity), lightDir(direction),
		cutOffAngle(theta), smoothAngle(theta) {
		++num;
	}

	SpotLight(std::string name, glm::vec3 position, glm::vec3 direction, glm::vec3 attenuation, float theta, float gamma,
			  float maxDistance = -1.0f, glm::vec3 color = glm::vec3(1.0f), float ambIntensity = 0.2f,
			  float diffIntensity = 1.0f)
		: PointLight(name, SPOT_LIGHT_ID, position, attenuation, maxDistance, color, ambIntensity, diffIntensity), lightDir(direction),
		cutOffAngle(theta), smoothAngle(gamma) {
		++num;
		assert(gamma >= theta);
	}

	virtual ~SpotLight() { }

	glm::vec3 getDirection() const { return lightDir; }

	float getCutOffAngle() const { return cutOffAngle; }

	float getSmoothingAngle() const { return smoothAngle; }

	void setDirection(glm::vec3 direction) {
		this->setDirty();
		lightDir = direction;
	}

	void setCutOffAngle(float phi) {
		this->setDirty();
		cutOffAngle = phi;
	}

	void setSmoothingAngle(float gamma) {
		this->setDirty();
		smoothAngle = gamma;
	}

	virtual void use(unsigned int shaderID, int index);

	virtual void ImGui();

	virtual void ImGui() const;
};
