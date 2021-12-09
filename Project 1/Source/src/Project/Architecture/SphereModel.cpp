
#pragma warning(push)
#pragma warning(disable : 26451)

#include <glm/glm.hpp>

#include <vector>

#include "SphereModel.h"

SphereModel::SphereModel(std::shared_ptr<Material> m, glm::mat4 xForm, std::string name, float r) : model(m, xForm, name), radius(r) {
	constexpr float precision = glm::pi<float>() / 31;
	constexpr float pi = glm::pi<float>();
	constexpr float halfPi = pi / 2.0f;
	constexpr float twoPi = pi * 2.0f;
	const int numOfVertsPerHalfCircle = glm::round(pi / precision) + 1;
	const int numOfVerts = numOfVertsPerHalfCircle * numOfVertsPerHalfCircle * 2 - numOfVertsPerHalfCircle;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;

	vertices.reserve(numOfVerts);
	normals.reserve(numOfVerts);
	texCoords.reserve(numOfVerts);

	for (float yaw = 0; yaw < twoPi + precision / 2.0f; yaw += precision) { //twoPi + prec/2 because we want 1.0 texCoords at the end of sphere
		for (float pitch = -halfPi; pitch < halfPi + precision / 2.0f; pitch += precision) {
			float x = r * glm::cos(yaw) * glm::cos(pitch);
			float y = r * glm::sin(yaw) * glm::cos(pitch);
			float z = r * glm::sin(pitch);

			glm::vec3 vert = glm::vec3(x, y, z);
			vertices.push_back(vert);
			glm::vec3 norm = vert / glm::length(vert);
			normals.push_back(norm);

			float ux = yaw / twoPi;
			float uy = (pitch + halfPi) / pi;

			texCoords.push_back(glm::vec2(ux, uy));
		}
	}

	std::vector<float> vboData;

	for (int i = 0; i < numOfVerts - numOfVertsPerHalfCircle; i++) {
		if (i % numOfVertsPerHalfCircle == numOfVertsPerHalfCircle - 1) //top of sphere
			continue;
		if (i % numOfVertsPerHalfCircle != numOfVertsPerHalfCircle - 2) { //not 1 down from the top
			glm::vec3 v[3];
			glm::vec3 n[3];
			glm::vec2 t[3];
			v[0] = vertices[i];
			n[0] = normals[i];
			t[0] = texCoords[i];
			v[1] = vertices[i + numOfVertsPerHalfCircle + 1];
			n[1] = normals[i + numOfVertsPerHalfCircle + 1];
			t[1] = texCoords[i + numOfVertsPerHalfCircle + 1];
			v[2] = vertices[i + 1];
			n[2] = normals[i + 1];
			t[2] = texCoords[i + 1];

			for (int j = 0; j < 3; j++) {
				vboData.push_back(v[j].x);
				vboData.push_back(v[j].y);
				vboData.push_back(v[j].z);
				vboData.push_back(n[j].x);
				vboData.push_back(n[j].y);
				vboData.push_back(n[j].z);
				vboData.push_back(t[j].s);
				vboData.push_back(t[j].t);
			}
		}

		if (i % numOfVertsPerHalfCircle) { //not bottom of circle
			glm::vec3 v[3];
			glm::vec3 n[3];
			glm::vec2 t[3];
			v[0] = vertices[i];
			n[0] = normals[i];
			t[0] = texCoords[i];
			v[1] = vertices[i + numOfVertsPerHalfCircle];
			n[1] = normals[i + numOfVertsPerHalfCircle];
			t[1] = texCoords[i + numOfVertsPerHalfCircle];
			v[2] = vertices[i + numOfVertsPerHalfCircle + 1];
			n[2] = normals[i + numOfVertsPerHalfCircle + 1];
			t[2] = texCoords[i + numOfVertsPerHalfCircle + 1];

			for (int j = 0; j < 3; j++) {
				vboData.push_back(v[j].x);
				vboData.push_back(v[j].y);
				vboData.push_back(v[j].z);
				vboData.push_back(n[j].x);
				vboData.push_back(n[j].y);
				vboData.push_back(n[j].z);
				vboData.push_back(t[j].s);
				vboData.push_back(t[j].t);
			}
		}
	}

	options.count = vboData.size() / 8LL;
	options.indexed = false;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, vboData.size() * sizeof(float), vboData.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (8 * sizeof(float)), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (8 * sizeof(float)), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, (8 * sizeof(float)), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

#pragma warning(pop)