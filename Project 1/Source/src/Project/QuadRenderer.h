#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "model.h"

class QuadRenderer : public model {
    // ------------------------------------------------------------------
    float vertices[20] = {
         0.5f,  0.5f, 0.0f, 0.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f, 1.0f, 1.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, // top left 
    };

protected:
    unsigned int indices[6] = {  // note that we start from 0!
        0, 3, 1,  // first Triangle
        1, 3, 2,   // second Triangle
    };

public:
    QuadRenderer(glm::mat4 xForm, Material* m) {
        // set up vertex data (and buffer(s)) and configure vertex attributes
        modelMatrix = xForm;

        material = m;

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, VBO);
        glGenBuffers(1, &EBO);
        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(VAO);


        // vertex buffer object, simple version, just coordinates

        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
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