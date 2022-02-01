#include "Line.h"
#include "SceneGraph.h"

void Line::init(glm::vec3 start, glm::vec3 end) {
	points = new float[6];

	points[0] = start.x;
	points[1] = start.y;
	points[2] = start.z;
	points[3] = end.x;
	points[4] = end.y;
	points[5] = end.z;

	options.renderType = GL_LINES;
	options.count = 2;
	options.indexed = false;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), points, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

std::pair<glm::vec3, glm::vec3> Line::getPointOnLine(float progress) {
	if (progress < 0.0f || progress > 1.0f)
		return {glm::vec3(), glm::vec3()};

	glm::quat result = glm::lerp(glm::quat(0.0f, getStartPoint()), glm::quat(0.0f, getEndPoint()), progress);
	return {glm::vec3(result.x, result.y, result.z), glm::normalize(getEndPoint() - getStartPoint())};
}

void Line::renderUpdate() {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, (unsigned long long)numOfPoints * 3 * sizeof(float), points);
	glBindVertexArray(0);
}
