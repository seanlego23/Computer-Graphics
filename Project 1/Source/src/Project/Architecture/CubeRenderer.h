#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "model.h"

class CubeRenderer : public model {

	float vertices[48] = {
		-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, //Back top left
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, //Back bottom left
		 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, //Back bottom right
		 0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f, //Back top right
		-0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 1.0f, //Front top left
		-0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, //Front bottom left
		 0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f, //Front bottom right
		 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, //Front top right
	};

protected:

	unsigned int indices[36] = {
		0, 3, 1,
		3, 2, 1,
		0, 1, 4,
		1, 5, 4,
		0, 4, 3,
		3, 4, 7,
		2, 6, 5,
		1, 2, 5,
		2, 3, 7,
		2, 7, 6,
		4, 6, 7,
		4, 5, 6,
	};

public:

	CubeRenderer(std::shared_ptr<Material> m, glm::mat4 xForm, std::string name) : model(m, xForm, name) {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO[0]);
		glGenBuffers(1, &EBO);
		// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
		glBindVertexArray(VAO);


		// vertex buffer object, simple version, just coordinates

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// set up the element array buffer containing the vertex indices for the "mesh"
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		options.count = sizeof(indices) / sizeof(unsigned int);

		// remember: do NOT unbind the EBO while a VAO is active, as the bound element buffer object IS stored in the VAO; keep the EBO bound.
		// don't be tempted to do this --->  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
		// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
		glBindVertexArray(0);
	}

};