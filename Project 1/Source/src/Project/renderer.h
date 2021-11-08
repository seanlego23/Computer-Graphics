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

    std::string name;
    bool instanced = false;
    bool isDirty = false;
    unsigned int instance_count = 1;
    unsigned int VBO[8] = {}, VAO = 0, EBO = 0;
    unsigned int indexCount = 0;
    double elapsedTime = 0;

    Material* material = nullptr;
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    renderer() { }

public:

    renderer(Material* m, glm::mat4 xForm, std::string name) {
        material = m;
        modelMatrix = xForm;
        this->name = name;
    }

    renderer(const renderer& rhs) : material(rhs.material), modelMatrix(rhs.modelMatrix), name(rhs.name + " Copy") { }

    virtual ~renderer() { }

    bool isInstanced() {
        return instanced;
    }

    //If the geometry has been updated, then this function will return true
    bool isDirty() {
        return isDirty;
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

    virtual void renderUpdate() { isDirty = false; }

    virtual void render(glm::mat4 vMat, glm::mat4 pMat, double deltaTime, SceneGraph* sg);

    virtual void OnTransform(glm::mat4 oldModel, glm::mat4 newModel) { }
};