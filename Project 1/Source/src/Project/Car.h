#pragma once

#include "Architecture\ObjModel.h"

class Car : public ObjModel {

	float currentProgress = 0.0f;
	float speed;
	int currentAttachmentIndex = 0;

	glm::mat4 originalXForm;

public:

	Car(std::shared_ptr<Material> m, glm::mat4 xForm, std::string name, const char* file, float s) : ObjModel(m, xForm, name, file), speed(s), 
		originalXForm(xForm) { }

	float getSpeed() const { return speed; }

	void setSpeed(float s) { speed = s; }

	virtual void update(double deltaTime);

};

