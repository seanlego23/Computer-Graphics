#include <imgui.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Light.h"

std::vector<std::shared_ptr<Light>> Light::lights;

unsigned int DirectionalLight::num = 0;
unsigned int PointLight::num = 0;
unsigned int SpotLight::num = 0;

std::string getLightType(unsigned short type_id) {
	std::string type;

	switch (type_id) {
	case GENERIC_LIGHT_ID:
		type = "Generic Light";
		break;
	case DIRECTIONAL_LIGHT_ID:
		type = "Directional Light";
		break;
	case POINT_LIGHT_ID:
		type = "Point Light";
		break;
	case SPOT_LIGHT_ID:
		type = "Spot Light";
		break;
	default:
		type = "Unknown Light Type";
		break;
	}

	return type;
}

void Light::ImGui() {
	if (!listed)
		return;
	
	float color[3] = { lightColor.r, lightColor.g, lightColor.b };
	float ambIntensity = ambientIntensity;
	float diffIntensity = diffuseIntensity;

	std::string type = getLightType(type_id);

	ImGui::Text(("Type: " + type).c_str());
	ImGui::DragFloat3("Color", color, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Ambient Intensity", &ambIntensity, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Diffuse Intensity", &diffuseIntensity, 0.01f, 0.0f, 1.0f);

	lightColor = glm::vec3(color[0], color[1], color[2]);
	ambientIntensity = ambIntensity;
	diffuseIntensity = diffIntensity;	
}

void Light::ImGui() const { 
	std::string type = getLightType(type_id);

	int r = lightColor.r * 100;
	int g = lightColor.g * 100;
	int b = lightColor.b * 100;
	int amb = ambientIntensity * 100;
	int diff = diffuseIntensity * 100;

	ImGui::Text(("Type: " + type).c_str());
	ImGui::Text(("Color: R: " + std::to_string(r) + "% G: " + std::to_string(g) + "% B: " + std::to_string(b) + "%").c_str());
	ImGui::Text(("Ambient Intensity: " + std::to_string(amb) + "%").c_str());
	ImGui::Text(("Diffuse Intensity: " + std::to_string(diff) + "%").c_str());
}

void DirectionalLight::use(unsigned int shaderID, int index) {
	std::string l = "ls.lights[" + std::to_string(index) + "]";
	glUniform3fv(glGetUniformLocation(shaderID, (l + ".color").c_str()), 1, glm::value_ptr(this->getColor()));
	glUniform1f(glGetUniformLocation(shaderID, (l + ".ambientIntensity").c_str()), this->getAmbientIntensity());
	glUniform1f(glGetUniformLocation(shaderID, (l + ".diffuseIntensity").c_str()), this->getDiffuseIntensity());
	glUniform3fv(glGetUniformLocation(shaderID, (l + ".direction").c_str()), 1, glm::value_ptr(lightDir));
	glUniform1i(glGetUniformLocation(shaderID, (l + ".type").c_str()), DIRECTIONAL_LIGHT_ID);
}

void DirectionalLight::ImGui() {
	if (!isImGuiListed())
		return;

	Light::ImGui();

	float dir[3] = {lightDir.x, lightDir.y, lightDir.z};

	ImGui::DragFloat3("Direction", dir, 0.01f, -1000.0f, 1000.0f);

	lightDir = glm::vec3(dir[0], dir[1], dir[2]);
}

void DirectionalLight::ImGui() const { 
	if (!isImGuiListed())
		return;

	Light::ImGui();

	ImGui::Text(("Direction: X: " + std::to_string(lightDir.x) + " Y: " + std::to_string(lightDir.y) + " Z: " + std::to_string(lightDir.z)).c_str());
}

void PointLight::use(unsigned int shaderID, int index) { }

void PointLight::ImGui() {
	if (!isImGuiListed())
		return;

	Light::ImGui();

	float pos[3] = {lightPos.x, lightPos.y, lightPos.z};
	float atten[3] = {att.x, att.y, att.z};

	ImGui::DragFloat3("Position", pos, 0.01f, -1000.0f, 1000.0f);
	ImGui::Text("Attenuation Coefficients");
	ImGui::InputFloat("Constant", &atten[0], 0.01f, 0.2f, "%.2f");
	ImGui::InputFloat("Linear", &atten[1], 0.001f, 0.02f, "%.4f");
	ImGui::InputFloat("Quadratic", &atten[2], 0.0001f, 0.002f, "%.6f");

	lightPos = glm::vec3(pos[0], pos[1], pos[2]);
	att = glm::vec3(atten[0], atten[1], atten[2]);
}

void PointLight::ImGui() const { 
	if (!isImGuiListed())
		return;

	Light::ImGui();

	ImGui::Text(("Position: X: " + std::to_string(lightPos.x) + " Y: " + std::to_string(lightPos.y) + " Z: " + std::to_string(lightPos.z)).c_str());
	ImGui::Text("Attenuation Coefficients");
	ImGui::Text(("Constant: " + std::to_string(att.x)).c_str());
	ImGui::Text(("Linear: " + std::to_string(att.y)).c_str());
	ImGui::Text(("Quadratic: " + std::to_string(att.z)).c_str());
}

void SpotLight::use(unsigned int shaderID, int index) { }

void SpotLight::ImGui() {
	if (!isImGuiListed())
		return;

	Light::ImGui();

	float pos[3] = {lightPos.x, lightPos.y, lightPos.z};
	float dir[3] = {lightDir.x, lightDir.y, lightDir.z};
	float theta = glm::degrees(cutOffAngle);
	float gamma = glm::degrees(smoothAngle);

	ImGui::DragFloat3("Position", pos, 0.01f, -1000.0f, 1000.0f);
	ImGui::DragFloat3("Direction", dir, 0.01f, -1000.0f, 1000.0f);
	ImGui::DragFloat("Cut Off Angle", &theta, 0.01f, 0.0f, 60.0f);
	ImGui::DragFloat("Smooth Edges Angle", &gamma, 0.01f, 0.0f, 85.0f);

	lightPos = glm::vec3(pos[0], pos[1], pos[2]);
	lightDir = glm::vec3(dir[0], dir[1], dir[2]);
	cutOffAngle = glm::radians(theta);
	smoothAngle = glm::radians(gamma);
}

void SpotLight::ImGui() const { 
	if (!isImGuiListed())
		return;

	Light::ImGui();

	ImGui::Text(("Position: X: " + std::to_string(lightPos.x) + " Y: " + std::to_string(lightPos.y) + " Z: " + std::to_string(lightPos.z)).c_str());
	ImGui::Text(("Direction: X: " + std::to_string(lightDir.x) + " Y: " + std::to_string(lightDir.y) + " Z: " + std::to_string(lightDir.z)).c_str());
	ImGui::Text(("Cut Off Angle: " + std::to_string(glm::degrees(cutOffAngle)) + " degrees").c_str());
	ImGui::Text(("Smooth Edges Angle: " + std::to_string(glm::degrees(smoothAngle)) + " degrees").c_str());
}
