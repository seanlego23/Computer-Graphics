#include <string>

#include "BezierCurve.h"
#include "SceneGraph.h"

void BezierCurve::calculateLines() {
	using namespace std;

	for (auto [t, point] = tuple{0.0f, 0}; t < 1.0f + precision / 2.0f; t += precision, point++) {
		float x = 0, y = 0, z = 0;
		for (int i = 0; i < numOfControlPoints; i++) {
			//use 1e-5 as numerical tolerance
			float multiplier = nChooseR(numOfControlPoints - 1, i) * (i != numOfControlPoints - 1 ? pow(1.0f - t, numOfControlPoints - i - 1) : 1) * (i != 0 ? pow(t, i) : 1);
			x += multiplier * controlPoints[i * 3];
			y += multiplier * controlPoints[i * 3 + 1];
			z += multiplier * controlPoints[i * 3 + 2];
		}
		points[point * 3] = x;
		points[point * 3 + 1] = y;
		points[point * 3 + 2] = z;
	}
}

BezierCurve::BezierCurve(std::shared_ptr<Material> m, glm::mat4 xForm, std::string name, glm::vec3 startPoint, glm::vec3 endPoint, float prec, 
						 calcFnc calcPoints) : Line(m, xForm, name) {

	calcPointsFnc = calcPoints;
	precision = prec; //Precision needs to be as exact as possible
	start = startPoint;
	end = endPoint;

	auto [cPoints, n] = calcPointsFnc(startPoint, endPoint);
	controlPoints = cPoints;
	numOfControlPoints = n;

	numOfPoints = round(1.0f / precision) + 1.0f;
	points = new float[(long long) numOfPoints * 3];

	calculateLines();

	options.renderType = GL_LINE_STRIP;
	options.count = numOfPoints;
	options.indexed = false;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, (unsigned long long)numOfPoints * 3 * sizeof(float), points, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

std::pair<glm::vec3, glm::vec3> BezierCurve::getPointOnLine(float progress) {
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
		if ((lsum + len)/ l > progress)
			break;
		lsum += len;
	}

	progress -= lsum / l;
	return {glm::normalize(p2 - p1) * progress * l + p1, glm::normalize(p2 - p1)};
}

void BezierCurve::render(glm::mat4 vMat, glm::mat4 pMat, double deltaTime, SceneGraph* sg) {
	this->pushShaderVariable("curvePrecision", GL_FLOAT, 1, &precision);

	renderer::render(vMat, pMat, deltaTime, sg);
}

std::pair<float*, unsigned int> eightPointRectangle(glm::vec3 start, glm::vec3 end) {
	float* points = new float[24];

	float midX = (start.x + end.x) / 2.0f;
	float midY = (start.y + end.y) / 2.0f;
	float midZ = (start.z + end.z) / 2.0f;

	points[0] = start.x;
	points[1] = start.y;
	points[2] = start.z;

	points[3] = start.x;
	points[4] = midY;
	points[5] = midZ;

	points[6] = midX;
	points[7] = start.y;
	points[8] = midZ;

	points[9] = start.x;
	points[10] = end.y;
	points[11] = end.z;

	points[12] = midX;
	points[13] = end.y;
	points[14] = end.z;

	points[15] = end.x;
	points[16] = start.y;
	points[17] = end.z;

	points[18] = end.x;
	points[19] = midY;
	points[20] = end.z;

	points[21] = end.x;
	points[22] = end.y;
	points[23] = end.z;

	return {points, 8};
}

std::pair<float*, unsigned int> fourPointRectangle(glm::vec3 start, glm::vec3 end) {
	float* points = new float[12];

	points[0] = start.x;
	points[1] = start.y;
	points[2] = start.z;

	points[3] = start.x;
	points[4] = end.y;
	points[5] = start.z;

	points[6] = end.x;
	points[7] = start.y;
	points[8] = end.z;

	points[9] = end.x;
	points[10] = end.y;
	points[11] = end.z;

	return {points, 4};
}
