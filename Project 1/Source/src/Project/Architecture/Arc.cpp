#pragma warning(push)
#pragma warning(disable : 26451)

#include "Arc.h"

Arc::Arc(std::shared_ptr<Material> m, glm::mat4 xForm, std::string name, glm::vec3 center, glm::vec3 start, float angle, float precision) 
	: Line(m, xForm, name) { 
	
	glm::vec3 radius = start - center;
	this->numOfPoints = glm::round(1.0f / precision) + 1;
	this->points = new float[this->numOfPoints * 3];

	float increment = angle * precision;
	points[0] = start.x;
	points[1] = start.y;
	points[2] = start.z;
	for (int i = 1; i < this->numOfPoints; i++) {
		glm::quat q(glm::cos(i * increment / 2), 0.0f, 0.0f, glm::sin(i * increment / 2));
		glm::quat q_(q.w, -q.x, -q.y, -q.z);
		glm::quat r(0.0f, radius);
		glm::quat s = q * r * q_;
		glm::vec3 p = glm::vec3(s.x, s.y, s.z) + center;
		points[i*3] = p.x;
		points[i*3+1] = p.y;
		points[i*3+2] = p.z;
	}

	options.renderType = GL_LINES;
	options.count = this->numOfPoints;
	options.indexed = false;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, this->numOfPoints * 3 * sizeof(float), points, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

float Arc::length() {
	float l = 0;
	for (int i = 0; i < this->numOfPoints-1; i++) {
		glm::vec3 p1(points[i * 3], points[i * 3 + 1], points[i * 3 + 2]);
		glm::vec3 p2(points[i * 3 + 3], points[i * 3 + 4], points[i * 3 + 5]);
		l += glm::length(p2 - p1);
	}
	return l;
}

std::pair<glm::vec3, glm::vec3> Arc::getPointOnLine(float progress) {
	if (progress < 0.0f || progress > 1.0f)
		return {glm::vec3(), glm::vec3()};

	float l = length();
	float lsum = 0.0f;
	int i = 0;
	glm::vec3 p1, p2;
	for (; i < numOfPoints - 1; i++) {
		p1 = glm::vec3(points[i * 3], points[i * 3 + 1], points[i * 3 + 2]);
		p2 = glm::vec3(points[i * 3 + 3], points[i * 3 + 4], points[i * 3 + 5]);
		float len = glm::length(p2 - p1);
		if ((lsum + len) / l > progress)
			break;
		lsum += len;
	}

	progress -= lsum / l;
	return {glm::normalize(p2 - p1) * progress * l + p1, glm::normalize(p2 - p1)};
}

#pragma warning(pop)