#include "renderer.h"

void renderer::render(glm::mat4 vMat, glm::mat4 pMat, double deltaTime, SceneGraph* sg) { // here's where the "actual drawing" gets done

    glm::mat4 mvp;

    unsigned int shaderID = material->use();

    mvp = pMat * vMat * modelMatrix;

    glUniformMatrix4fv(glGetUniformLocation(shaderID, "m"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "v"), 1, GL_FALSE, glm::value_ptr(vMat));
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "p"), 1, GL_FALSE, glm::value_ptr(pMat));

    glUniformMatrix4fv(glGetUniformLocation(shaderID, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform1i(glGetUniformLocation(shaderID, "instances"), instance_count);

    sg->light->use(shaderID);
    glUniform3f(glGetUniformLocation(shaderID, "cameraPos"), sg->camera.position.x, sg->camera.position.y, sg->camera.position.z);
    glUniform4f(glGetUniformLocation(shaderID, "material.ambient"), material->ambient.r, material->ambient.g, material->ambient.b, material->ambient.a);
    glUniform4f(glGetUniformLocation(shaderID, "material.diffuse"), material->diffuse.r, material->diffuse.g, material->diffuse.b, material->diffuse.a);
    glUniform4f(glGetUniformLocation(shaderID, "material.specular"), material->specular.r, material->specular.g, material->specular.b, material->specular.a);
    glUniform1f(glGetUniformLocation(shaderID, "material.shininess"), material->shininess);
    if (material->getTextureID()) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, material->getTextureID());
        glUniform1i(glGetUniformLocation(shaderID, "material.texture"), 0);
        glUniform1i(glGetUniformLocation(shaderID, "material.useTexture"), 1);
    } else
        glUniform1i(glGetUniformLocation(shaderID, "material.useTexture"), 0);
    glUniform1d(glGetUniformLocation(shaderID, "elapsedTime"), elapsedTime += deltaTime);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glBindVertexArray(VAO);

    if (!instanced)
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    else
        glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0, instance_count);
}