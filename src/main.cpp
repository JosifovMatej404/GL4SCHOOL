// Math Constants
#define _USE_MATH_DEFINES

// Local Headers
#include "OpenGLPrj.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>

void framebuffer_size_callback(GLFWwindow* mWindow, int width, int height);
void processInput(GLFWwindow* window);

const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    void main(){
        gl_Position = vec4(aPos, 1.0f);
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    void main(){
        FragColor = vec4(0.8509803922f, 0, 0, 1.0f);
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

    glGetProgramiv(shaderProgram, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

std::vector<float> generateOuterCircle(int quallity) {
    std::vector<float> vertices;

    float outerRadius = 0.9f;
    float innerRadius = 0.75f;

    float zConstant = 0.0f;
    float angle = 0.0f;
    float step = 2 * M_PI / quallity;

    float scalingFactor = 0.7f;

    for (int i = 0; i < quallity + 2; i++) {
        float x = 0;
        float y = 0;

        if (i % 2 == 0) {
            x = innerRadius * cos(angle);
            y = innerRadius * sin(angle) * scalingFactor;
        }
        else {
            x = outerRadius * cos(angle);
            y = outerRadius * sin(angle) * scalingFactor;
        }
        
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(zConstant);

        angle += step;
    }

    return vertices;
}

std::vector<float> generateVerticalCircle(int quallity) {
    std::vector<float> vertices;

    float outerRadius = 0.525f;
    float innerRadius = 0.375f;

    float zConstant = 0.0f;
    float angle = 0.0f;
    float step = 2 * M_PI / quallity;

    float scalingFactor = 0.3f;

    for (int i = 0; i < quallity + 2; i++) {
        float x = 0;
        float y = 0;

        if (i % 2 == 0) {
            x = innerRadius * cos(angle) * scalingFactor;
            y = innerRadius * sin(angle);
        }
        else {
            x = outerRadius * cos(angle) * scalingFactor;
            y = outerRadius * sin(angle);
        }

        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(zConstant);

        angle += step;
    }

    return vertices;
}

std::vector<float> generateInnerCircle(int quallity) {
    std::vector<float> vertices;

    float outerRadius = 0.675f;
    float innerRadius = 0.525f;

    float zConstant = 0.0f;
    float angle = 0.0f;
    float step = 2 * M_PI / quallity;

    float scalingFactor = 0.5f;

    std::vector<float> center = { 0, 0.375f };

    for (int i = 0; i < quallity + 2; i++) {
        if (angle < M_PI) {
            angle += step;
            continue;
        }
        
        float x = 0;
        float y = 0;

        if (i % 2 == 0) {
            x = center[0] + innerRadius * cos(angle);
            y = center[1] + innerRadius * sin(angle) * scalingFactor;
        }
        else {
            x = center[0] + outerRadius * cos(angle);
            y = center[1] + outerRadius * sin(angle) * scalingFactor;
        }

        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(zConstant);

        angle += step;
    }

    return vertices;
}

std::vector<std::vector<float>> generateToyotaLogo(int quallity) {
    std::vector<std::vector<float>> ellipses(3);
    ellipses[0] = generateOuterCircle(quallity);
    ellipses[1] = generateVerticalCircle(quallity);
    ellipses[2] = generateInnerCircle(quallity);
    return ellipses;
}

int main(int argc, char * argv[]) {

    // Load GLFW and Create a Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    auto mWindow = glfwCreateWindow(mWidth, mHeight, "OpenGL", nullptr, nullptr);

    // Check for Valid Context
    if (mWindow == nullptr) {
        fprintf(stderr, "Failed to Create OpenGL Context");
        return EXIT_FAILURE;
    }

    // Create Context and Load OpenGL Functions
    glfwMakeContextCurrent(mWindow);
    glfwSetFramebufferSizeCallback(mWindow, framebuffer_size_callback);
    gladLoadGL();

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

    auto shaderProgram = createShaderProgram();

    std::vector<std::vector<float>> ellipses = generateToyotaLogo(100);

    std::vector<unsigned int> VBOs(ellipses.size()), VAOs(ellipses.size());

    glGenVertexArrays(ellipses.size(), VAOs.data());
    glGenBuffers(ellipses.size(), VBOs.data());

    for (int i = 0; i < ellipses.size(); i++) {
        glBindVertexArray(VAOs[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, ellipses[i].size() * sizeof(float), ellipses[i].data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);
    }

    // Rendering Loop
    while (glfwWindowShouldClose(mWindow) == false) {
        processInput(mWindow);

        glClearColor(1, 1, 1, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        for (int i = 0; i < ellipses.size(); i++) {
            glBindVertexArray(VBOs[i]);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, ellipses[i].size() / 3);
        }

        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }   

    glDeleteVertexArrays(ellipses.size(), VAOs.data());
    glDeleteBuffers(ellipses.size(), VBOs.data());

    glfwTerminate();
    
    return EXIT_SUCCESS;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* mWindow, int width, int height) {
    glViewport(0, 0, width, height);
}
