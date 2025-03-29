#define _USE_MATH_DEFINES
// Local Headers
#include "OpenGLPrj.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    void main() {
        gl_Position = vec4(aPos, 1.0);
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    void main() {
        FragColor = vec4(1.0f, 0.5019607843f, 0.2f, 1.0f); // Yellow
    }
)";

GLuint createShaderProgram() {
    int success;
    char infoLog[512];

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }


    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;

}

GLFWwindow* createWindow() {
    // Load GLFW and Create a Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    return glfwCreateWindow(mWidth, mHeight, "OpenGL", nullptr, nullptr);
}

void createContext(GLFWwindow* mWindow) {
    // Create Context and Load OpenGL Functions
    glfwMakeContextCurrent(mWindow);
    glfwSetFramebufferSizeCallback(mWindow, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));
}

std::vector<std::vector<float>> generateFlower(int leafCount, float startingAngle, float innerRadius, float outerRadius) {
    std::vector<std::vector<float>> leafs(leafCount);
   
    float step = 2 * M_PI / leafCount;
    float smallStep = 2 * M_PI / (leafCount * 3);

    for (int i = 0; i < leafCount; i++) {
        leafs[i].emplace_back(0);
        leafs[i].emplace_back(0);
        leafs[i].emplace_back(0);

        leafs[i].emplace_back(innerRadius * cos(startingAngle - smallStep));
        leafs[i].emplace_back(innerRadius * sin(startingAngle - smallStep));
        leafs[i].emplace_back(0);

        leafs[i].emplace_back(outerRadius * cos(startingAngle));
        leafs[i].emplace_back(outerRadius * sin(startingAngle));
        leafs[i].emplace_back(0);

        leafs[i].emplace_back(innerRadius * cos(startingAngle + smallStep));
        leafs[i].emplace_back(innerRadius * sin(startingAngle + smallStep));
        leafs[i].emplace_back(0);

        startingAngle += step;
    }
    return leafs;
}

int main(int argc, char * argv[]) {

    auto mWindow = createWindow();

    if (mWindow == nullptr) {
        fprintf(stderr, "Failed to Create OpenGL Context");
        return EXIT_FAILURE;
    }

    createContext(mWindow);

    float innerRadius = 0.4f;
    float outerRadius = 0.9f;
    int leafCount = 8;

    std::vector<std::vector<float>> flower = generateFlower(leafCount, 0, innerRadius, outerRadius);

    std::vector<unsigned int> VBOs(flower.size()), VAOs(flower.size());

    glGenVertexArrays(flower.size(), VAOs.data());
    glGenBuffers(flower.size(), VBOs.data());

    for (int i = 0; i < flower.size(); i++) {
        glBindVertexArray(VAOs[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, flower[i].size() * sizeof(float), flower[i].data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);
    }

    std::vector<std::vector<float>> Lflower = generateFlower(leafCount*2, M_PI/leafCount, innerRadius, outerRadius);

    std::vector<unsigned int> LVBOs(Lflower.size()), LVAOs(Lflower.size());

    glGenVertexArrays(Lflower.size(), LVAOs.data());
    glGenBuffers(Lflower.size(), LVBOs.data());

    for (int i = 0; i < Lflower.size(); i++) {
        glBindVertexArray(LVAOs[i]);
        glBindBuffer(GL_ARRAY_BUFFER, LVBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, Lflower[i].size() * sizeof(float), Lflower[i].data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);
    }

    auto shaderProgram = createShaderProgram();

    while (glfwWindowShouldClose(mWindow) == false) {

        glClearColor(0.2f, 0.2980392157f, 0.2980392157f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        for (int i = 0; i < flower.size(); i++) {
            glBindVertexArray(VAOs[i]);
            glDrawArrays(GL_TRIANGLE_FAN, 0, flower[i].size() / 3);
        }

        for (int i = 0; i < Lflower.size(); i++) {
            glBindVertexArray(LVAOs[i]);
            glDrawArrays(GL_LINE_LOOP, 0, Lflower[i].size() / 3);
        }

        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }   

    glDeleteVertexArrays(flower.size(), VAOs.data());
    glDeleteBuffers(flower.size(), VBOs.data());

    glDeleteVertexArrays(Lflower.size(), LVAOs.data());
    glDeleteBuffers(Lflower.size(), LVBOs.data());

    glfwTerminate();

    return EXIT_SUCCESS;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
