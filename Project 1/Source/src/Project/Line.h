#pragma once

#include "model.h"

class Line : public model {

	float lineColor[8]{};

protected:

	float* points = nullptr;
	unsigned int numOfPoints = 2;

	Line() { }

private:

	void init(glm::vec3 start, glm::vec3 end);

public:

	Line(Material* m, glm::mat4 xForm, glm::vec3 start, glm::vec3 end) {
		material = m;
		modelMatrix = xForm;

		init(start, end);
	}

	virtual ~Line() {
		if (points)
			delete[] points;
	}

	std::pair<float*, unsigned int> getPoints() { return {points, numOfPoints}; }

	unsigned int getNumOfPoints() { return numOfPoints; }

	virtual glm::vec3 getStartPoint() { return glm::vec3(points[0], points[1], points[2]); }

	virtual glm::vec3 getEndPoint() { return glm::vec3(points[3], points[4], points[5]); }

	virtual void update();

	virtual void setStartPoint(glm::vec3 start) {
		points[0] = start.x;
		points[1] = start.y;
		points[2] = start.z;

		Line::update();
	}

	virtual void setEndPoint(glm::vec3 end) {
		points[3] = end.x;
		points[4] = end.y;
		points[5] = end.z;

		Line::update();
	}

	virtual void render(glm::mat4 vMat, glm::mat4 pMat, double deltaTime, SceneGraph* sg);

};