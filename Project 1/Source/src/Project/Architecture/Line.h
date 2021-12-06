#pragma once
#pragma warning(push)
#pragma warning(disable : 26451)

#include "model.h"

class Line : public model {

	float lineColor[8]{};

protected:

	float* points = nullptr;
	unsigned int numOfPoints = 2;

	Line(std::shared_ptr<Material> m, glm::mat4 xForm, std::string name) : model(m, xForm, name) { }

private:

	void init(glm::vec3 start, glm::vec3 end);

public:

	Line() = delete;

	Line(std::shared_ptr<Material> m, glm::mat4 xForm, std::string name, glm::vec3 start, glm::vec3 end) : model(m, xForm, name) {
		init(start, end);
	}

	Line(const Line& rhs) : model(rhs), numOfPoints(rhs.numOfPoints) {
		points = new float[numOfPoints * 3];
		memcpy(points, rhs.points, numOfPoints * 3 * sizeof(float));
	}

	virtual ~Line() {
		if (points)
			delete[] points;
	}

	std::pair<float*, unsigned int> getPoints() { return {points, numOfPoints}; }

	unsigned int getNumOfPoints() { return numOfPoints; }

	virtual glm::vec3 getStartPoint() { return glm::vec3(points[0], points[1], points[2]); }

	virtual glm::vec3 getEndPoint() { return glm::vec3(points[3], points[4], points[5]); }

	virtual void renderUpdate();

	virtual void setStartPoint(glm::vec3 start) {
		points[0] = start.x;
		points[1] = start.y;
		points[2] = start.z;
		this->setDirty();
	}

	virtual void setEndPoint(glm::vec3 end) {
		points[3] = end.x;
		points[4] = end.y;
		points[5] = end.z;
		this->setDirty();
	}

};

#pragma warning(pop)