#pragma once

#include "ObjModel.h"

class Car : public ObjModel {

	float speed;

public:

	Car(Material* m, glm::mat4 xForm, const char* file, float s) : ObjModel(m, xForm, file), speed(s) { }

	float getSpeed() const { return speed; }

	void setSpeed(float s) { speed = s; }

};

