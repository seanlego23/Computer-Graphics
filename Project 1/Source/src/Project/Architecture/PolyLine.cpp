#include "PolyLine.h"

PolyLine::PolyLine(std::shared_ptr<Material> m, glm::mat4 xForm, std::string name, unsigned int nPoints, glm::vec3 pts ...) : Line(m, xForm, name) { 

}

PolyLine::PolyLine(const PolyLine& rhs) : Line(rhs) { }
