#pragma once

#include <glm/glm.hpp>

#include "Line.h"
#include "Material.h"

class PolyLine : public Line {
protected:

	PolyLine() { }

public:

	PolyLine(Material* m, glm::mat4 xForm, unsigned int nPoints, glm::vec3 pts...);

	PolyLine(const PolyLine& rhs);

	glm::vec3 getPoint(unsigned int index) {
		assert(index >= 0 || index < this->getNumOfPoints());
		return glm::vec3(points[index*3], points[index*3+1], points[index*3+2]);
	}

};

