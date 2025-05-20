#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Shader.hpp"
#include "Sphere.hpp"
#include "Cube.hpp"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void renderQuad();

unsigned int quadVAO = 0;
unsigned int quadVBO;

int main() {
    // GLFW init
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Three Objects Scene with Bloom", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader shaderMetal("../res/shaders/shader.vert", "../res/shaders/metal.frag");
    Shader shaderMatte("../res/shaders/shader.vert", "../res/shaders/matte.frag");
    Shader shaderGlass("../res/shaders/shader.vert", "../res/shaders/glass.frag");
    Shader shaderBlur("../res/shaders/quad.vert", "../res/shaders/blur.frag");
    Shader shaderFinal("../res/shaders/quad.vert", "../res/shaders/final.frag");
    Shader shaderBrightExtract("../res/shaders/quad.vert", "../res/shaders/extract_light.frag");

    shaderBrightExtract.use();
    shaderBrightExtract.setInt("scene", 0);

    Cube cube1, cube2;
    unsigned int cube1VAO = cube1.getVAO();
    unsigned int cube2VAO = cube2.getVAO();
    Sphere sphere(32, 32);
    unsigned int sphereVAO = sphere.getVAO();
    int sphereCount = sphere.getIndexCount();

    // HDR framebuffer setup
    unsigned int hdrFBO;
    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

    unsigned int colorBuffers[2];
    glGenTextures(2, colorBuffers);
    for (unsigned int i = 0; i < 2; i++) {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Clamp to edge to avoid bleeding on blur
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
    }
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "HDR Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Ping-pong framebuffers for blur
    unsigned int pingpongFBO[2], pingpongColorbuffers[2];
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);
    for (unsigned int i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Pingpong Framebuffer " << i << " not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    while (!glfwWindowShouldClose(window)) {
        float time = glfwGetTime();
        processInput(window);

        glm::vec3 lightPos(cos(time) * 3.0f, 2.0f, 3.0f);
        glm::vec3 viewPos(0.0f, 0.0f, 5.0f);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(viewPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        // 1. Render scene into floating point framebuffer (HDR)
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw metal cube
        glm::mat4 model1 = glm::rotate(glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.5f, 0.0f)), glm::vec3(0.8f)), time / 2, glm::vec3(0.0f, -1.0f, 0.0f));
        shaderMetal.use();
        shaderMetal.setMat4("model", model1);
        shaderMetal.setMat4("view", view);
        shaderMetal.setMat4("projection", projection);
        shaderMetal.setVec3("lightPos", lightPos);
        shaderMetal.setVec3("viewPos", viewPos);
        glBindVertexArray(cube2VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Draw matte cube
        glm::mat4 model2 = glm::scale(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)), time, glm::vec3(0.5f, 1.0f, 0.0f)), glm::vec3(0.8f));
        shaderMatte.use();
        shaderMatte.setMat4("model", model2);
        shaderMatte.setMat4("view", view);
        shaderMatte.setMat4("projection", projection);
        shaderMatte.setVec3("lightPos", lightPos);
        shaderMatte.setVec3("viewPos", viewPos);
        glBindVertexArray(cube1VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Draw glass sphere
        glm::mat4 model3 = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-0.3f, -0.3f, 0.0f)), glm::vec3(glm::sin(time)));
        shaderGlass.use();
        shaderGlass.setMat4("model", model3);
        shaderGlass.setMat4("view", view);
        shaderGlass.setMat4("projection", projection);
        shaderGlass.setVec3("lightPos", lightPos);
        shaderGlass.setVec3("viewPos", viewPos);
        glBindVertexArray(sphereVAO);
        glDrawElements(GL_TRIANGLES, sphereCount, GL_UNSIGNED_INT, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. Extract bright parts of the scene and render to pingpongFBO[0]
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[0]);
        glClear(GL_COLOR_BUFFER_BIT);
        shaderBrightExtract.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
        renderQuad();

        // 3. Blur bright fragments with ping-pong framebuffers
        bool horizontal = true, first_iteration = true;
        int amount = 5;
        for (int i = 0; i < amount; i++) {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            shaderBlur.use();
            shaderBlur.setBool("horizontal", horizontal);
            glBindTexture(GL_TEXTURE_2D, first_iteration ? pingpongColorbuffers[0] : pingpongColorbuffers[!horizontal]);
            renderQuad();
            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 4. Final composite pass: render quad with original scene and bloom blur texture
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderFinal.use();
        shaderFinal.setInt("scene", 0);
        shaderFinal.setInt("bloomBlur", 1);
        shaderFinal.setBool("bloom", true);
        shaderFinal.setFloat("exposure", 1.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
        renderQuad();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void renderQuad() {
    if (quadVAO == 0) {
        float quadVertices[] = {
            // positions        // texture coords
            -1.0f,  1.0f,       0.0f, 1.0f,
            -1.0f, -1.0f,       0.0f, 0.0f,
             1.0f, -1.0f,       1.0f, 0.0f,

            -1.0f,  1.0f,       0.0f, 1.0f,
             1.0f, -1.0f,       1.0f, 0.0f,
             1.0f,  1.0f,       1.0f, 1.0f
        };
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
