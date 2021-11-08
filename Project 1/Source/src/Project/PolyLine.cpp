#include "PolyLine.h"

PolyLine::PolyLine(Material* m, glm::mat4 xForm, unsigned int nPoints, glm::vec3 pts ...) : Line() { 
	material = m;
	modelMatrix = xForm;
}

PolyLine::PolyLine(const PolyLine& rhs) : Line(rhs) { }
