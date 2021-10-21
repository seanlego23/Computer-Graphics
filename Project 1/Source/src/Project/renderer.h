#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Material.h"
#include "shader_s.h"

class renderer {

protected:

    bool instanced = false;
    unsigned int instance_count = 1;
    unsigned int VBO[8] = {}, VAO = 0, EBO = 0;
    unsigned int indexCount = 0;
    double elapsedTime = 0;

    Material* material;
    glm::mat4 modelMatrix;

public:

    bool isInstanced() {
        return instanced;
    }

    void setInstanced(bool inst) {
        instanced = inst;
    }

    unsigned int getInstanceCount() {
        return instance_count;
    }

    void setInstanceCount(unsigned int count) {
        instance_count = count;
    }

    void setXForm(glm::mat4 mat) {
        modelMatrix = mat;
    }

    void rotate(const float axis[], const float angle) {
        modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(axis[0], axis[1], axis[2]));
    } 

    void translate(const float trans[]) {
        modelMatrix = glm::translate(modelMatrix, glm::vec3(trans[0], trans[1], trans[2]));
    }

    void scale(const float scale[]) {
        modelMatrix = glm::scale(modelMatrix, glm::vec3(scale[0], scale[1], scale[2]));
    }

    virtual void render(glm::mat4 vMat, glm::mat4 pMat, double deltaTime) { // here's where the "actual drawing" gets done

        glm::mat4 mvp;

        unsigned int shaderID = material->use();

        //rotate(glm::value_ptr(glm::vec3(0.0f, 0.0f, 1.0f)), deltaTime); // easter egg!  rotate incrementally with delta time

        mvp = pMat * vMat * modelMatrix;

        glUniformMatrix4fv(glGetUniformLocation(shaderID, "m"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "v"), 1, GL_FALSE, glm::value_ptr(vMat));
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "p"), 1, GL_FALSE, glm::value_ptr(pMat));

        glUniformMatrix4fv(glGetUniformLocation(shaderID, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
        myShader->setInt("instances", instance_count);
        myShader->setVec4("material.ambient", material->ambient.r, material->ambient.g, material->ambient.b, material->ambient.a);
        myShader->setVec4("material.diffuse", material->diffuse.r, material->diffuse.g, material->diffuse.b, material->diffuse.a);
        myShader->setVec4("material.specular", material->specular.r, material->specular.g, material->specular.b, material->specular.a);
        myShader->setFloat("material.shininess", material->shininess);
        if (material->textureID) { //If textureID != 0
            glBindTexture(GL_TEXTURE_2D, material->textureID);
            myShader->setInt("material.texture", 0);
        }
        myShader->setDouble("elapsedTime", elapsedTime += deltaTime);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        glBindVertexArray(VAO);

        if (!instanced)
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        else 
            glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0, instance_count);
    }

    virtual void OnTransform(glm::mat4 oldModel, glm::mat4 newModel) { }
};