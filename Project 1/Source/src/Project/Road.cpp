#include "Road.h"
#include "SceneGraph.h"

#pragma warning(push)
#pragma warning(disable : 6001 6386 6385 26451)

/* Calculates the intersection between the lines (p1,p2) and (p3,p4)
* Returns the ratio between the intersection point and (p2 - p1)
* Multiply the ratio by the vector (p2 - p1) and add to p1, and you get the intersection point.
*/
float Road::calculateIntersection(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4) {
	float diffX1 = p2.x - p1.x;
	float diffX2 = p3.x - p4.x;
	float diffY1 = p2.y - p1.y;
	float diffY2 = p3.y - p4.y;
	float diffZ1 = p2.z - p1.z;
	float diffZ2 = p3.z - p4.z;

	float u;

	if (!diffX1 != !diffX2) {
		if (!diffX1)
			u = (p1.x - p4.x) / diffX2;
		else
			return (p4.x - p1.x) / diffX1;
	} else if (!diffY1 != !diffY2) {
		if (!diffY1)
			u = (p1.y - p4.y) / diffY2;
		else
			return (p4.y - p1.y) / diffY1;
	} else if (!diffZ1 != !diffZ2) {
		if (!diffZ1)
			u = (p1.z - p4.z) / diffZ2;
		else
			return (p4.z - p1.z) / diffZ1;
	} else {
		if (diffX1 && diffX2) {
			if (diffY1 && diffY2)
				return (diffY2 * (p4.x - p1.x) + diffX2 * (p1.y - p4.y)) / (diffX1 * diffY2 - diffX2 * diffY1);
			else
				return (diffZ2 * (p4.x - p1.x) + diffX2 * (p1.z - p4.z)) / (diffX1 * diffZ2 - diffX2 * diffZ1);
		} else
			return (diffZ2 * (p4.y - p1.y) + diffY2 * (p1.z - p4.z)) / (diffY1 * diffZ2 - diffY2 * diffZ1);
	}
	if (diffX1 && diffX2)
		return (p4.x + u * diffX2 - p1.x) / diffX1;
	else if (diffY1 && diffY2)
		return (p4.y - u * diffY2 - p1.y) / diffY1;
	else
		return (p4.z - u * diffZ2 - p1.z) / diffZ1;
}

bool Road::calculateOffset(std::vector<glm::vec3>& offsetPoints, bool positiveOffset) {
	glm::vec3 signedOffsetDir[2] = {(positiveOffset ? 1.0f : -1.0f) * offsetDir[0], (positiveOffset ? 1.0f : -1.0f) * offsetDir[1]};

	auto [curvePoints, numOfCurvePoints] = centerCurve->getPoints();

	unsigned int numOfOffsetCurvePoints = numOfCurvePoints * 2 - 2;
	float* offsetCurves = new float[numOfOffsetCurvePoints * 3];
	const glm::vec3 front = glm::normalize(glm::cross(offsetDir[0], normal)); //Assume the original offset direction is perpendicular to the curve to the left

	//Offset the each line in the curve by a offset in a direction
	for (int i = 0; i < numOfCurvePoints - 1; i++) { //iterates number of lines
		glm::mat3 rotationMatrix = glm::mat3(1.0f);

		glm::vec3 v2 = glm::normalize(glm::vec3(curvePoints[(i + 1) * 3] - curvePoints[i * 3], curvePoints[(i + 1) * 3 + 1] - curvePoints[i * 3 + 1], curvePoints[(i + 1) * 3 + 2] - curvePoints[i * 3 + 2]));
		float dot = glm::dot(front, v2);
		if (abs(1 + dot) < 1e-6)
			rotationMatrix = glm::mat3(-1.0f);
		else {
			glm::vec3 n = glm::cross(front, v2);
			glm::mat3 n_x = glm::mat3(0, n.z, -n.y, -n.z, 0, n.x, n.y, -n.x, 0);
			rotationMatrix += n_x + n_x * n_x * (1.0f / (1.0f + dot)); //If dot=-1, then rotationMatrix = -I as above.
		}

		//The offset direction needs to be rotated to match the rotation from the front vector to the i'th line.
		glm::vec3 rotatedOffset = glm::normalize(rotationMatrix * signedOffsetDir[0]) * curveOffset;

		offsetCurves[i * 6] = curvePoints[i * 3] + rotatedOffset.x;
		offsetCurves[i * 6 + 1] = curvePoints[i * 3 + 1] + rotatedOffset.y;
		offsetCurves[i * 6 + 2] = curvePoints[i * 3 + 2] + rotatedOffset.z;

		offsetCurves[i * 6 + 3] = curvePoints[i * 3 + 3] + rotatedOffset.x;
		offsetCurves[i * 6 + 4] = curvePoints[i * 3 + 4] + rotatedOffset.y;
		offsetCurves[i * 6 + 5] = curvePoints[i * 3 + 5] + rotatedOffset.z;
	}

	glm::vec3 start(curvePoints[0], curvePoints[1], curvePoints[2]);

	float t = calculateIntersection(start, start + signedOffsetDir[0], glm::vec3(offsetCurves[0], offsetCurves[1], offsetCurves[2]),
									glm::vec3(offsetCurves[3], offsetCurves[4], offsetCurves[5]));
	offsetPoints.push_back(start + t * signedOffsetDir[0]);

	//Join all of the offset curves including trimming and extending curves to meet.
	for (int i = 0; i < numOfCurvePoints - 2; i++) {
		glm::vec3 p1 = offsetPoints.back();
		glm::vec3 p2 = glm::vec3(offsetCurves[i * 6 + 3], offsetCurves[i * 6 + 4], offsetCurves[i * 6 + 5]);
		glm::vec3 p3 = glm::vec3(offsetCurves[i * 6 + 6], offsetCurves[i * 6 + 7], offsetCurves[i * 6 + 8]);
		glm::vec3 p4 = glm::vec3(offsetCurves[i * 6 + 9], offsetCurves[i * 6 + 10], offsetCurves[i * 6 + 11]);
		
		if (glm::length(p3 - p2) < 1e-6) {
			offsetPoints.push_back(p2);
			continue;
		}
		t = calculateIntersection(p1, p2, p3, p4);

		if (t < 0) {
			while (t < 0) {
				offsetPoints.pop_back();
				if (!offsetPoints.size())
					return false;
				p2 = p1;
				p1 = offsetPoints.back();
				t = calculateIntersection(p1, p2, p3, p4);
			}
		}

		offsetPoints.push_back(p1 + t * (p2 - p1));
	}

	glm::vec3 end(curvePoints[numOfCurvePoints * 3 - 3], curvePoints[numOfCurvePoints * 3 - 2], curvePoints[numOfCurvePoints * 3 - 1]);
	t = calculateIntersection(end, end + signedOffsetDir[1],
							  glm::vec3(offsetCurves[numOfOffsetCurvePoints * 3 - 6], offsetCurves[numOfOffsetCurvePoints * 3 - 5], offsetCurves[numOfOffsetCurvePoints * 3 - 4]),
							  glm::vec3(offsetCurves[numOfOffsetCurvePoints * 3 - 3], offsetCurves[numOfOffsetCurvePoints * 3 - 2], offsetCurves[numOfOffsetCurvePoints * 3 - 1]));

	offsetPoints.push_back(end + t * signedOffsetDir[1]);

	delete[] offsetCurves;
	return true;
}

Road::Road(Material* m, glm::mat4 xForm, Line* curve, glm::vec3 startNormal, glm::vec3 startOffsetDir, glm::vec3 endOffsetDir, float offset,
		   float texturePrecision, bool renderCurve) : centerCurve(curve), normal(startNormal), offsetDir{startOffsetDir, endOffsetDir}, 
		   curveOffset(offset), renderCenterCurve(renderCurve) {
	material = m;
	modelMatrix = xForm;

	std::vector<glm::vec3> side1OffsetPoints, side2OffsetPoints;
	calculateOffset(side1OffsetPoints, true);
	calculateOffset(side2OffsetPoints, false);

	assert(side1OffsetPoints.size() == side2OffsetPoints.size());

	std::vector<float> vbovalues;
	std::vector<unsigned int> indices;

	float side1TexY = 0.0f, side2TexY = 0.0f;

	for (int i = 0; i < side1OffsetPoints.size() - 1; i++) {
		glm::vec3 faceNormal = glm::normalize(glm::cross(side2OffsetPoints[i] - side1OffsetPoints[i],
														 side2OffsetPoints[i+1] - side1OffsetPoints[i]));

		vbovalues.push_back(side1OffsetPoints[i].x);
		vbovalues.push_back(side1OffsetPoints[i].y);
		vbovalues.push_back(side1OffsetPoints[i].z);

		vbovalues.push_back(faceNormal.x);
		vbovalues.push_back(faceNormal.y);
		vbovalues.push_back(faceNormal.z);

		vbovalues.push_back(side1TexY);
		vbovalues.push_back(0.0f);

		indices.push_back(i*6);

		vbovalues.push_back(side2OffsetPoints[i].x);
		vbovalues.push_back(side2OffsetPoints[i].y);
		vbovalues.push_back(side2OffsetPoints[i].z);

		vbovalues.push_back(faceNormal.x);
		vbovalues.push_back(faceNormal.y);
		vbovalues.push_back(faceNormal.z);

		vbovalues.push_back(side2TexY);
		vbovalues.push_back(1.0f);

		indices.push_back(i*6+1);

		vbovalues.push_back(side2OffsetPoints[i+1].x);
		vbovalues.push_back(side2OffsetPoints[i+1].y);
		vbovalues.push_back(side2OffsetPoints[i+1].z);

		vbovalues.push_back(faceNormal.x);
		vbovalues.push_back(faceNormal.y);
		vbovalues.push_back(faceNormal.z);

		float length = glm::length((side2OffsetPoints[i+1] - side2OffsetPoints[i]));

		vbovalues.push_back(side2TexY += texturePrecision * length);
		vbovalues.push_back(1.0f);

		indices.push_back(i*6+2);

		faceNormal = glm::normalize(glm::cross(side2OffsetPoints[i+1] - side1OffsetPoints[i],
											   side1OffsetPoints[i+1] - side2OffsetPoints[i]));

		vbovalues.push_back(side1OffsetPoints[i].x);
		vbovalues.push_back(side1OffsetPoints[i].y);
		vbovalues.push_back(side1OffsetPoints[i].z);

		vbovalues.push_back(faceNormal.x);
		vbovalues.push_back(faceNormal.y);
		vbovalues.push_back(faceNormal.z);

		vbovalues.push_back(side1TexY);
		vbovalues.push_back(0.0f);

		indices.push_back(i*6+3);

		vbovalues.push_back(side2OffsetPoints[i+1].x);
		vbovalues.push_back(side2OffsetPoints[i+1].y);
		vbovalues.push_back(side2OffsetPoints[i+1].z);

		vbovalues.push_back(faceNormal.x);
		vbovalues.push_back(faceNormal.y);
		vbovalues.push_back(faceNormal.z);

		vbovalues.push_back(side2TexY);
		vbovalues.push_back(1.0f);

		indices.push_back(i*6+4);

		vbovalues.push_back(side1OffsetPoints[i+1].x);
		vbovalues.push_back(side1OffsetPoints[i+1].y);
		vbovalues.push_back(side1OffsetPoints[i+1].z);

		vbovalues.push_back(faceNormal.x);
		vbovalues.push_back(faceNormal.y);
		vbovalues.push_back(faceNormal.z);

		length = glm::length((side1OffsetPoints[i + 1] - side1OffsetPoints[i]));

		vbovalues.push_back(side1TexY += texturePrecision * length);
		vbovalues.push_back(0.0f);

		indices.push_back(i*6+5);
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, vbovalues.size() * sizeof(float), vbovalues.data(), GL_STATIC_DRAW);

	indexCount = indices.size();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

void Road::render(glm::mat4 vMat, glm::mat4 pMat, double deltaTime, SceneGraph* sg) { 
	renderer::render(vMat, pMat, deltaTime, sg);

	if (renderCenterCurve) {
		centerCurve->setXForm(this->modelMatrix);
		centerCurve->render(vMat, pMat, deltaTime, sg);
	}
}

#pragma warning(pop)