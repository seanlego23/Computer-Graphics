#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <map>
#include <string>

#include "Texture.h"

class Material {

	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;

	Shader* shader;

	float shininess;
	unsigned int textureID = 0;
	std::string name;

public:

	static std::map<std::string, Material*> materials;

	Material(glm::vec4 amb, glm::vec4 diff, glm::vec4 spec, float shine, unsigned int texID, Shader* s, std::string n) :
		ambient(amb), diffuse(diff), specular(spec), shininess(shine), textureID(texID), shader(s), name(n) {

		materials[name] = this;
	}

	unsigned int use() {
		shader->use();
		return shader->ID;
	}
};