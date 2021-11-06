#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Material.h"
#include "SceneGraph.h"
#include "shader_s.h"

class SceneGraph;

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

    virtual ~renderer() { }

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

    virtual void renderer::render(glm::mat4 vMat, glm::mat4 pMat, double deltaTime, SceneGraph* sg);

    virtual void OnTransform(glm::mat4 oldModel, glm::mat4 newModel) { }
};