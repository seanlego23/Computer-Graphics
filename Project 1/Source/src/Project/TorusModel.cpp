
//#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "TorusModel.h"

TorusModel::TorusModel(float holeRadius, float crossXRadius, Shader* s, glm::mat4 xForm, Material* m) { 
	myShader = s;
	modelMatrix = xForm;
	material = m;

	constexpr float precision = glm::pi<float>() / 15.0f; //There will be a point every pi/15 radians or 12 degrees
	constexpr float twoPi = glm::pi<float>() * 2.0f;
	const int numOfVertsPerCircle = round(2 * glm::pi<float>() / precision);
	const long numOfVerts = numOfVertsPerCircle * numOfVertsPerCircle;
	vertices.reserve(12LL * numOfVerts); //3 floats per vertex
	indices.reserve(6LL * numOfVerts); //We create 2 triangles for every vertex, see below.

	int i = 0;
	//twoPi - precision / 2.0f allows for floating point errors
	for (float hAngle = 0; hAngle < twoPi - precision / 2.0f; hAngle += precision) {
		for (float xAngle = 0; xAngle < twoPi - precision / 2.0f; xAngle += precision, i++) {
			float x = (holeRadius + crossXRadius) * cos(hAngle) + crossXRadius * cos(xAngle) * cos(hAngle);
			float y = (holeRadius + crossXRadius) * sin(hAngle) + crossXRadius * cos(xAngle) * sin(hAngle);
			float z = crossXRadius * sin(xAngle);

			float nx = x - (holeRadius + crossXRadius) * cos(hAngle);//wrong
			float ny = y - (holeRadius + crossXRadius) * sin(hAngle);//wrong
			float nz = z;

			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);
			vertices.push_back(nx);
			vertices.push_back(ny);
			vertices.push_back(nz);
			vertices.push_back(1.0f - 0.5f * cos(hAngle));
			vertices.push_back(0.5f - 0.5f * sin(hAngle));
			vertices.push_back(0.75f - 0.25f * cos(xAngle));
			vertices.push_back(1.0f);
			vertices.push_back(0.5f + 0.5f * sin(hAngle));
			vertices.push_back(0.5f + 0.5f * sin(xAngle));
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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(10 * sizeof(float)));
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), indices.data(), GL_STATIC_DRAW);

	indexCount = indices.size();

	glBindVertexArray(0);
}
