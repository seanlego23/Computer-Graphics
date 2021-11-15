#pragma once

#include "Architecture\ObjModel.h"

class Car : public ObjModel {

	float speed;

public:

	Car(std::shared_ptr<Material> m, glm::mat4 xForm, std::string name, const char* file, float s) : ObjModel(m, xForm, name, file), speed(s) { }

	float getSpeed() const { return speed; }

	void setSpeed(float s) { speed = s; }

};

