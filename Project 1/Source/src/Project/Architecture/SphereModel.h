#pragma once
#include "model.h"
class SphereModel : public model {

	float radius;

public:

	SphereModel() = delete;

	SphereModel(std::shared_ptr<Material> m, glm::mat4 xForm, std::string name, float r);

};

