
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

BezierCurve::BezierCurve(Material* m, glm::mat4 xForm, glm::vec3 startPoint, glm::vec3 endPoint, float prec, calcFnc calcPoints) { 
	material = m;
	modelMatrix = xForm;
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

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, (unsigned long long)numOfPoints * 3 * sizeof(float), points, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void BezierCurve::render(glm::mat4 vMat, glm::mat4 pMat, double deltaTime, SceneGraph* sg) { 
	glm::mat4 mvp;

	unsigned int shaderID = material->use();

	mvp = pMat * vMat * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(shaderID, "m"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "v"), 1, GL_FALSE, glm::value_ptr(vMat));
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "p"), 1, GL_FALSE, glm::value_ptr(pMat));

	glUniformMatrix4fv(glGetUniformLocation(shaderID, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));

	glUniform1f(glGetUniformLocation(shaderID, "curvePrecision"), 0.04f);

	if (material != nullptr) {
		//glUniform4f(glGetUniformLocation(shaderID, "material.ambient"), material->ambient.r, material->ambient.g, material->ambient.b, material->ambient.a);
		//glUniform4f(glGetUniformLocation(shaderID, "material.ambient"), material->diffuse.r, material->diffuse.g, material->diffuse.b, material->diffuse.a);
		//glUniform4f(glGetUniformLocation(shaderID, "material.ambient"), material->specular.r, material->specular.g, material->specular.b, material->specular.a);
		//glUniform1f(glGetUniformLocation(shaderID, "material.shininess"), material->shininess);
		if (material->getTextureID()) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, material->getTextureID());
			glUniform1i(glGetUniformLocation(shaderID, "material.texture"), 0);
		}
	}
	glUniform1d(glGetUniformLocation(shaderID, "elapsedTime"), elapsedTime += deltaTime);

	glBindVertexArray(VAO);

	glDrawArrays(GL_LINE_STRIP, 0, numOfPoints);
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
