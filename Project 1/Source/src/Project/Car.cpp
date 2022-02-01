#include "Car.h"
#include "Architecture/Line.h"

void Car::update(double deltaTime) { 
	currentProgress += speed * deltaTime;
	if (currentProgress > 1.0f) {
		currentAttachmentIndex++;
		if (currentAttachmentIndex == attachedModels.size())
			currentAttachmentIndex = 0;
		currentProgress -= 1.0f;
	}

	attachment* currentAttachment = &attachedModels[currentAttachmentIndex];
	Line* currentLine = reinterpret_cast<Line*>(currentAttachment->getAttachedModel());
	std::pair<glm::vec3, glm::vec3> loc = currentLine->getPointOnLine(currentProgress);

	glm::vec3 up(0.0f, 0.0f, 1.0f);
	glm::mat4 cTransform = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f / 3.0f, 1.0f / 3.0f, 1.0f / 3.0f));
	cTransform = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * cTransform;
	cTransform = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), up) * cTransform;
	cTransform = glm::rotate(glm::mat4(1.0f), glm::acos(glm::dot(glm::vec3(1.0f, 0.0f, 0.0f), glm::normalize(loc.second))), up) * cTransform;
	cTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f)) * cTransform;
	cTransform = glm::translate(glm::mat4(1.0f), loc.first)*cTransform;
	this->setXForm(cTransform);
}
