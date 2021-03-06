#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <map>
#include <string>

#include "shader_s.h"

class Material {
	std::shared_ptr<Shader> shader;

	unsigned int textureID = 0;
	std::string name;

public:

	static std::map<std::string, std::shared_ptr<Material>> materials;

	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	float shininess;

	Material(glm::vec4 amb, glm::vec4 diff, glm::vec4 spec, float shine, unsigned int texID, std::shared_ptr<Shader> s, std::string n) :
		ambient(amb), diffuse(diff), specular(spec), shininess(shine), textureID(texID), shader(s), name(n) {

		materials[name] = std::shared_ptr<Material>(this);
	}

	unsigned int use() {
		shader->use();
		return shader->ID;
	}

	unsigned int getTextureID() { return textureID; }

};
