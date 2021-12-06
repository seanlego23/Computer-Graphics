#include "renderer.h"

#pragma warning(push)
#pragma warning(disable : 26451 4312)

void renderer::pushShaderVariable(const char* name, GLenum type, unsigned int count, const void* var) {
    void* variable = new __int32[count];
    memcpy(variable, var, count * sizeof(__int32)); //Every GLenum type used is 32 bits wide
    shaderVariables.push_back({name, type, count, 1, 1, 1, false, false, variable});
}

void renderer::pushShaderVariableArray(const char* name, GLenum type, unsigned int arrayCount, unsigned int count, const void* var) {
    void* variable = new __int32[count * arrayCount];
    memcpy(variable, var, count * arrayCount * sizeof(__int32));
    shaderVariables.push_back({name, type, count, arrayCount, 1, 1, false, false, variable});
}

void renderer::pushShaderMatrixArray(const char* name, unsigned int arrayCount, unsigned char col, unsigned char row, bool transpose, const GLfloat* var) { 
    void* variable = new float[col * row * arrayCount];
    memcpy(variable, var, col * row * arrayCount * sizeof(float));
    shaderVariables.push_back({name, GL_FLOAT, 0, arrayCount, col, row, transpose, true, variable});
}

void renderer::useShaderVariables(unsigned int shaderID) { 
    for (shaderVariable var : shaderVariables) {
        GLint loc = glGetUniformLocation(shaderID, var.name);
        if (var.matrix) {
            float* values = reinterpret_cast<float*>(var.var);
            if (var.col == var.row) {
                switch (var.col) {
                case 2:
                    glUniformMatrix2fv(loc, var.array_count, var.transpose, values);
                    break;
                case 3:
                    glUniformMatrix3fv(loc, var.array_count, var.transpose, values);
                    break;
                case 4:
                    glUniformMatrix4fv(loc, var.array_count, var.transpose, values);
                    break;
                }
            } else {
                switch (var.row) {
                case 2:
                    if (var.col == 3)
                        glUniformMatrix2x3fv(loc, var.array_count, var.transpose, values);
                    else if (var.col == 4)
                        glUniformMatrix2x4fv(loc, var.array_count, var.transpose, values);
                    break;
                case 3:
                    if (var.col == 2)
                        glUniformMatrix3x2fv(loc, var.array_count, var.transpose, values);
                    else if (var.col == 4)
                        glUniformMatrix3x4fv(loc, var.array_count, var.transpose, values);
                    break;
                case 4:
                    if (var.col == 2)
                        glUniformMatrix4x2fv(loc, var.array_count, var.transpose, values);
                    else if (var.col == 3)
                        glUniformMatrix4x3fv(loc, var.array_count, var.transpose, values);
                }
            }
        } else {
            switch (var.type) {
            case GL_INT:
            {
                int* values = reinterpret_cast<int*>(var.var);
                switch (var.count) {
                case 1:
                    glUniform1iv(loc, var.array_count, values);
                    break;
                case 2:
                    glUniform2iv(loc, var.array_count, values);
                    break;
                case 3:
                    glUniform3iv(loc, var.array_count, values);
                    break;
                case 4:
                    glUniform4iv(loc, var.array_count, values);
                    break;
                }
                break;
            }
            case GL_UNSIGNED_INT:
            {
                unsigned int* values = reinterpret_cast<unsigned int*>(var.var);
                switch (var.count) {
                case 1:
                    glUniform1uiv(loc, var.array_count, values);
                    break;
                case 2:
                    glUniform2uiv(loc, var.array_count, values);
                    break;
                case 3:
                    glUniform3uiv(loc, var.array_count, values);
                    break;
                case 4:
                    glUniform4uiv(loc, var.array_count, values);
                    break;
                }
                break;
            }
            case GL_FLOAT:
            {
                float* values = reinterpret_cast<float*>(var.var);
                switch (var.count) {
                case 1:
                    glUniform1fv(loc, var.array_count, values);
                    break;
                case 2:
                    glUniform2fv(loc, var.array_count, values);
                    break;
                case 3:
                    glUniform3fv(loc, var.array_count, values);
                    break;
                case 4:
                    glUniform4fv(loc, var.array_count, values);
                    break;
                }
                break;
            }
            }
        }
    }
}

void renderer::render(glm::mat4 vMat, glm::mat4 pMat, double deltaTime, SceneGraph* sg) { // here's where the "actual drawing" gets done

    if (this->isDirty()) {
        renderUpdate();
        renderer::renderUpdate(); //Called to handle any lower level updates and cleanups
    }

    glm::mat4 mvp;

    unsigned int shaderID;
    if (sg->pass == SceneGraph::RenderPass::GEOMETRY) {
        std::shared_ptr<Shader> geo = Shader::shaders["geometry"];
        geo->use();
        shaderID = geo->ID;
    } else if (sg->pass == SceneGraph::RenderPass::LIGHTING) {
        std::shared_ptr<Shader> light = Shader::shaders["light"];
        light->use();
        shaderID = light->ID;

        glUniform1i(glGetUniformLocation(shaderID, "ls.nLights"), Light::lights.size());
        for (int i = 0; i < Light::lights.size(); i++) {
            Light::lights[i]->use(shaderID, i);
        }

        //unsigned int lightIndex = glGetUniformBlockIndex(shaderID, "Lights");
        //glUniformBlockBinding(shaderID, lightIndex, 0);
    } else
        shaderID = material->use();

    if (sg->pass == SceneGraph::RenderPass::POST) {
        glUniform1f(glGetUniformLocation(shaderID, "exposure"), sg->exposure);
    }

    mvp = pMat * vMat * modelMatrix;

    glUniformMatrix4fv(glGetUniformLocation(shaderID, "m"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "v"), 1, GL_FALSE, glm::value_ptr(vMat));
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "p"), 1, GL_FALSE, glm::value_ptr(pMat));

    glUniformMatrix4fv(glGetUniformLocation(shaderID, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform1i(glGetUniformLocation(shaderID, "instances"), options.instance_count);

    glUniform3f(glGetUniformLocation(shaderID, "cameraPos"), sg->camera.position.x, sg->camera.position.y, sg->camera.position.z);
    glUniform4f(glGetUniformLocation(shaderID, "material.ambient"), material->ambient.r, material->ambient.g, material->ambient.b, material->ambient.a);
    glUniform4f(glGetUniformLocation(shaderID, "material.diffuse"), material->diffuse.r, material->diffuse.g, material->diffuse.b, material->diffuse.a);
    glUniform4f(glGetUniformLocation(shaderID, "material.specular"), material->specular.r, material->specular.g, material->specular.b, material->specular.a);
    glUniform1f(glGetUniformLocation(shaderID, "material.shininess"), material->shininess);
    if (material->getTextureID() && sg->pass != SceneGraph::RenderPass::LIGHTING) {
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

    if (options.indexed) {
        if (!options.instanced)
            glDrawElements(options.renderType, options.count, options.indexType, (void*)options.offset);
        else
            glDrawElementsInstanced(options.renderType, options.count, options.indexType, (void*)options.offset, options.instance_count);
    } else {
        if (!options.instanced)
            glDrawArrays(options.renderType, options.offset, options.count);
        else 
            glDrawArraysInstanced(options.renderType, options.offset, options.count, options.instance_count);
    }
}

#pragma warning(pop)