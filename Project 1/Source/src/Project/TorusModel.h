#pragma once

#include <glm/glm.hpp>

#include <vector>

#include "Material.h"
#include "model.h"

class TorusModel : public model {

	std::vector<float> vertices;
	std::vector<int> indices;
	
public:

	TorusModel(float holeRadius, float crossXRadius, glm::mat4 xForm, Material* m);

};