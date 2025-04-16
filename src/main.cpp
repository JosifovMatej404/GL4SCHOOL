#define PI 3.14159265358979323846
#include <OpenGLPrj.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void checkCompileErrors(GLuint shader, std::string type);

const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;

const char* vertexShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;
out vec2 position;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    position = aPos.xy; // Pass local position
}
)glsl";

const char* fragmentShaderSource = R"glsl(
#version 330 core

uniform float time;
in vec2 position;
out vec4 FragColor;

float highFreq = 25.0; // small waves
float lowFreq = 5.0;   // big wave shape
float speed = 5.0;

vec3 colorA = vec3(1.0, 0.3, 0.0); // bright orange
vec3 colorB = vec3(0.8, 0.1, 0.1); // deep orange-red

void main()
{
    float amp = 0.5 + 0.5 * sin(position.y * lowFreq + time * speed); // [0,1]
    float wave = sin(position.y * highFreq + time * speed);          // small wave motion
    float combined = amp * wave;

    float t = 0.5 + 0.5 * combined;
    vec3 finalColor = mix(colorA, colorB, t);

    FragColor = vec4(finalColor, 1.0);
}
)glsl";



GLuint createShaderProgram() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    checkCompileErrors(vertexShader, "VERTEX");

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    checkCompileErrors(fragmentShader, "FRAGMENT");

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkCompileErrors(shaderProgram, "PROGRAM");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

void checkCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << type << " SHADER COMPILATION ERROR:\n" << infoLog << std::endl;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "PROGRAM LINKING ERROR:\n" << infoLog << std::endl;
        }
    }
}

std::vector<float> generateBar(std::vector<float> center, float height, float width) {
    std::vector<float> barVertices;
    float x = center[0];
    float y = center[1];

    barVertices = {
        x - width / 2, y - height / 2, 0.0f,
        x + width / 2, y - height / 2, 0.0f,
        x + width / 2, y + height / 2, 0.0f,
        x - width / 2, y + height / 2, 0.0f
    };

    return barVertices;
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Gradient Shader", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    GLuint shaderProgram = createShaderProgram();

    std::vector<float> vertices = generateBar({ 0.0f, 0.0f }, 2.0f, 2.0f);

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Optional wireframe

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        float time = glfwGetTime();
        glUniform1f(glGetUniformLocation(shaderProgram, "time"), time);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size() / 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
