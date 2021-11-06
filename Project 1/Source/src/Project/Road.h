#pragma once

#include <vector>

#include "Line.h"
#include "model.h"

class Road : public model {
	
	Line* centerCurve;
	glm::vec3 normal;
	glm::vec3 offsetDir[2];
	float curveOffset;
	bool renderCenterCurve;

	float calculateIntersection(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4);
	
	void calculateOffset(std::vector<glm::vec3>& offsetPoints, bool positiveOffset);

public:

	//Texture precision means the texture coords will increment by texturePrecision every unit length
	Road(Material* m, glm::mat4 xForm, Line* curve, glm::vec3 startNormal, glm::vec3 startOffsetDir, glm::vec3 endOffsetDir, float offset,
		 float texturePrecision, bool renderCurve);

	virtual void render(glm::mat4 vMat, glm::mat4 pMat, double deltaTime, SceneGraph* sg);

};

