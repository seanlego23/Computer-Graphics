
//#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "TorusModel.h"

TorusModel::TorusModel(float holeRadius, float crossXRadius, glm::mat4 xForm, Material* m) { 
	modelMatrix = xForm;
	material = m;

	constexpr float precision = glm::pi<float>() / 15.0f; //There will be a point every pi/15 radians or 12 degrees
	constexpr float twoPi = glm::pi<float>() * 2.0f;
	const int numOfVertsPerCircle = round(2 * glm::pi<float>() / precision);
	const long numOfVerts = numOfVertsPerCircle * numOfVertsPerCircle;
	vertices.reserve(8LL * numOfVerts);
	indices.reserve(6LL * numOfVerts);

	int i = 0;
	//twoPi - precision / 2.0f allows for floating point errors
	for (float holeAngle = 0; holeAngle < twoPi - precision / 2.0f; holeAngle += precision) {
		for (float crossXAngle = 0; crossXAngle < twoPi - precision / 2.0f; crossXAngle += precision, i++) {
			float x = (holeRadius + crossXRadius) * cos(holeAngle) + crossXRadius * cos(crossXAngle) * cos(holeAngle);
			float y = (holeRadius + crossXRadius) * sin(holeAngle) + crossXRadius * cos(crossXAngle) * sin(holeAngle);
			float z = crossXRadius * sin(crossXAngle);

			float nx = x - (holeRadius + crossXRadius) * cos(holeAngle);
			float ny = y - (holeRadius + crossXRadius) * sin(holeAngle);
			float nz = z;

			glm::vec3 normal(nx, ny, nz);
			normal = glm::normalize(normal);

			//vertex position
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);

			//vertex normal
			vertices.push_back(normal.x);
			vertices.push_back(normal.y);
			vertices.push_back(normal.z);

			//texture coords *** This algorithm doesn't extend the coords around the cross section correctly
			//If it did, it would create a texture artifact on the inside of the torus (via a compressed texture in between pi and pi + precision)
			//TODO: Fix
			vertices.push_back(0.5f + 0.5f * sin(holeAngle));
			vertices.push_back(0.5f + 0.5f * sin(crossXAngle - glm::pi<float>()));
		}
	}

	for (int i = 0; i < numOfVerts; i++) {
		//Indices for triangle to the bottom right of the point i
		indices.push_back(i);
		if (i % numOfVertsPerCircle == 0)
			indices.push_back(i + numOfVertsPerCircle - 1);
		else
			indices.push_back(i - 1);
		indices.push_back((i + numOfVertsPerCircle) % numOfVerts);
		//Indices for triangle to the top left of the point i
		indices.push_back(i);
		if ((i + 1) % numOfVertsPerCircle == 0)
			indices.push_back(i + 1 - numOfVertsPerCircle);
		else
			indices.push_back(i + 1);
		if (i >= numOfVertsPerCircle)
			indices.push_back((i - numOfVertsPerCircle) % numOfVerts);
		else
			indices.push_back(numOfVerts - numOfVertsPerCircle + (i % numOfVertsPerCircle));
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), indices.data(), GL_STATIC_DRAW);

	indexCount = indices.size();

	glBindVertexArray(0);
}
