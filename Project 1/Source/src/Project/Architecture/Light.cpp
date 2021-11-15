#include <imgui.h>

#include "Light.h"

std::vector<Light*> Light::lights;

unsigned int DirectionalLight::num = 0;
unsigned int PointLight::num = 0;
unsigned int SpotLight::num = 0;

void Light::ImGui() {
	if (!listed)
		return;
	
	float amb[3] = {ambient.r, ambient.g, ambient.b};
	float diff[3] = {diffuse.r, diffuse.g, diffuse.b};
	float spec[3] = {specular.r, specular.g, specular.b};
	float intensity = lightIntensity;

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

	ImGui::Text(("Type: " + type).c_str());
	ImGui::DragFloat3("Ambient Color", amb, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat3("Diffuse Color", diff, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat3("Specular Color", spec, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Intensity", &intensity, 1.0f, 0.0f, 1000.0f);

	ambient = glm::vec3(amb[0], amb[1], amb[2]);
	diffuse = glm::vec3(diff[0], diff[1], diff[2]);
	specular = glm::vec3(spec[0], spec[1], spec[2]);
	lightIntensity = intensity;
}

void DirectionalLight::ImGui() { 
	if (!isImGuiListed())
		return;

	Light::ImGui();

	float dir[3] = {lightDir.x, lightDir.y, lightDir.z};

	ImGui::DragFloat3("Direction", dir, 0.01f, -1000.0f, 1000.0f);

	lightDir = glm::vec3(dir[0], dir[1], dir[2]);
}

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