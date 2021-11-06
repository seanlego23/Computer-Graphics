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

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), points, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void Line::update() {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, (unsigned long long)numOfPoints * 3 * sizeof(float), points);
	glBindVertexArray(0);
}

void Line::render(glm::mat4 vMat, glm::mat4 pMat, double deltaTime, SceneGraph* sg) {
	glm::mat4 mvp;

	unsigned int shaderID = material->use();

	mvp = pMat * vMat * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(shaderID, "m"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "v"), 1, GL_FALSE, glm::value_ptr(vMat));
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "p"), 1, GL_FALSE, glm::value_ptr(pMat));

	glUniformMatrix4fv(glGetUniformLocation(shaderID, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
	glUniform1i(glGetUniformLocation(shaderID, "instances"), instance_count);

	glUniform4f(glGetUniformLocation(shaderID, "material.ambient"), material->ambient.r, material->ambient.g, material->ambient.b, material->ambient.a);
	glUniform4f(glGetUniformLocation(shaderID, "material.diffuse"), material->diffuse.r, material->diffuse.g, material->diffuse.b, material->diffuse.a);
	glUniform4f(glGetUniformLocation(shaderID, "material.specular"), material->specular.r, material->specular.g, material->specular.b, material->specular.a);
	glUniform1f(glGetUniformLocation(shaderID, "material.shininess"), material->shininess);
	if (material->getTextureID()) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material->getTextureID());
		glUniform1i(glGetUniformLocation(shaderID, "material.texture"), 0);
	}

	glBindVertexArray(VAO);

	glDrawArrays(GL_LINES, 0, 2);
}
