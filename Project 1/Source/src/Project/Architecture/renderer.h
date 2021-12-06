#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Material.h"
#include "Object.h"
#include "SceneGraph.h"
#include "shader_s.h"

class SceneGraph;

struct renderOptions {
    bool instanced;
    bool indexed;
    GLenum renderType;
    GLenum indexType;
    unsigned int instance_count;
    unsigned int count;
    unsigned int offset;
};

class renderer : public Object {
private:

    struct shaderVariable {
        const char* name;
        GLenum type;
        unsigned int count;
        unsigned int array_count;
        unsigned char col;
        unsigned char row;
        bool transpose;
        bool matrix;
        void* var;
    };

    std::vector<shaderVariable> shaderVariables;

    std::shared_ptr<Material> material;
    glm::mat4 modelMatrix = glm::mat4(1.0f);

protected:

    unsigned int VBO[8] = {}, VAO = 0, EBO = 0;
    double elapsedTime = 0;

    renderOptions options = {false, true, GL_TRIANGLES, GL_UNSIGNED_INT, 1, 0, 0};

public:

    renderer() { }

    renderer(std::shared_ptr<Material> m, glm::mat4 xForm, std::string name) : Object(name), material(m), modelMatrix(xForm) { }

    renderer(const renderer& rhs) : Object(rhs), material(rhs.material), modelMatrix(rhs.modelMatrix) { }

    virtual ~renderer() { }

protected:

    void pushShaderVariable(const char* name, GLenum type, unsigned int count, const void* var);

    void pushShaderVariableArray(const char* name, GLenum type, unsigned int arrayCount, unsigned int count, const void* var);

    void pushShaderMatrixArray(const char* name, unsigned int arrayCount, unsigned char col, unsigned char row, bool transpose, const GLfloat *var);

private:

    void useShaderVariables(unsigned int shaderID);

public:

    bool isInstanced() {
        return options.instanced;
    }

    void setInstanced(bool inst) {
        options.instanced = inst;
    }

    unsigned int getInstanceCount() {
        return options.instance_count;
    }

    void setInstanceCount(unsigned int count) {
        options.instance_count = count;
    }

    std::shared_ptr<Material> getMaterial() { return material; }

    void setMaterial(std::shared_ptr<Material> m) { material = m; }

    glm::mat4 getXForm() { return modelMatrix; }

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

    virtual void renderUpdate() { this->resetDirty(); }

    virtual void render(glm::mat4 vMat, glm::mat4 pMat, double deltaTime, SceneGraph* sg);

    virtual void OnTransform(glm::mat4 oldModel, glm::mat4 newModel) { }
};