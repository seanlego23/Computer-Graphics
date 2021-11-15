#pragma once

#include <glm/glm.hpp>

#include <vector>

#include "Material.h"
#include "model.h"

class TorusModel : public model {

	std::vector<float> vertices;
	std::vector<int> indices;
	
public:

	TorusModel() = delete;

	TorusModel(std::shared_ptr<Material> m, glm::mat4 xForm, std::string name, float holeRadius, float crossXRadius);

};