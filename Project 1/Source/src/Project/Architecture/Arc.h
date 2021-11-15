#pragma once

#include "Line.h"

class Arc : public Line {
protected:

	Arc(std::shared_ptr<Material> m, glm::mat4 xForm, std::string name) : Line(m, xForm, name) { }

public:

	Arc() = delete;

	Arc(std::shared_ptr<Material> m, glm::mat4 xForm, std::string name, glm::vec3 center, glm::vec3 start, glm::vec3 end, float radius, float precision) 
		: Line(m, xForm, name) {

	}

	Arc(const Arc& rhs) : Line(rhs) { }

	virtual ~Arc() { }

	virtual glm::vec3 getEndPoint() { return glm::vec3(points[numOfPoints*3-3], points[numOfPoints*3-2], points[numOfPoints*3-1]); }

};

