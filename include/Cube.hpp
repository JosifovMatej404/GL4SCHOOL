#ifndef CUBE_HPP
#define CUBE_HPP

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Cube {
public:
    unsigned int VAO, VBO;
    unsigned int indexCount;

    Cube() {
        float vertices[] = {
           -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
           -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
           -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

           // front face
           -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
           -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
           -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

           // left face
           -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
           -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
           -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
           -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
           -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
           -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

           // right face
            0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
            0.5f,  0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f,
            0.5f, -0.5f,  0.5f, 1.0f,  0.0f,  0.0f,

            // bottom face
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f,
             0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f,
             0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f,
             0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f,

            // top face
            -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,  0.0f,
             0.5f,  0.5f,  0.5f, 0.0f, 1.0f,  0.0f,
             0.5f,  0.5f, -0.5f, 0.0f, 1.0f,  0.0f,
             0.5f,  0.5f,  0.5f, 0.0f, 1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,  0.0f
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0); // position
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1); // normal
    }

    void Draw() const {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }

    unsigned int getVAO() const {
        return VAO;
    }
    unsigned int getIndexCount() const {
        return indexCount;
    }
    ~Cube() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
};

#endif