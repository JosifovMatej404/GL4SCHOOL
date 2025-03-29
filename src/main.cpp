#define _USE_MATH_DEFINES
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <cmath>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;

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
        FragColor = vec4(0, 0, 0, 1.0f); // Black color
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

std::vector<std::vector<float>> generateRingSegments(float innerRadius, float outerRadius, int segmentsPerSlice) {
    std::vector<std::vector<float>> segments;

    for (int i = 0; i < 6; i += 2) { // Only even segments (0, 2, 4)
        float startAngle = (i * M_PI / 3);
        float endAngle = ((i + 1) * M_PI / 3);
        float angleStep = (endAngle - startAngle) / segmentsPerSlice;

        std::vector<float> vertices;
        for (int j = 0; j <= segmentsPerSlice; j++) {
            float angle = startAngle + j * angleStep;
            float xOuter = outerRadius * cos(angle);
            float yOuter = outerRadius * sin(angle);
            float xInner = innerRadius * cos(angle);
            float yInner = innerRadius * sin(angle);

            vertices.push_back(xOuter);
            vertices.push_back(yOuter);
            vertices.push_back(0.0f);

            vertices.push_back(xInner);
            vertices.push_back(yInner);
            vertices.push_back(0.0f);
        }
        segments.push_back(vertices);
    }
    return segments;
}

std::vector<float> generateCenterCircle(float radius, int quallity) {
    std::vector<float> vertices;
    float angleStep = 2 * M_PI / quallity;
    float angle = 0.0f;
    float zConstant = 0.0f;
    for (int i = 0; i < quallity; i++) {
        float x = radius * cos(angle);
        float y = radius * sin(angle);
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(zConstant);
        angle += angleStep;
    }
    return vertices;
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Radioactive Ring", nullptr, nullptr);
    if (!window) {
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

    GLuint shaderProgram = createShaderProgram();

    float innerRadius = 0.3f;
    float outerRadius = 0.9f;
    int segmentsPerSlice = 800;

    std::vector<std::vector<float>> ringSegments = generateRingSegments(innerRadius, outerRadius, segmentsPerSlice);

    std::vector<GLuint> VAOs(ringSegments.size()), VBOs(ringSegments.size());
    glGenVertexArrays(ringSegments.size(), VAOs.data());
    glGenBuffers(ringSegments.size(), VBOs.data());

    for (size_t i = 0; i < ringSegments.size(); i++) {
        glBindVertexArray(VAOs[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, ringSegments[i].size() * sizeof(float), ringSegments[i].data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);
    }

    float radius = 0.2f;

    std::vector<float> circleVertices = generateCenterCircle(radius, segmentsPerSlice);

    unsigned int CVAO, CVBO;
    glGenVertexArrays(1, &CVAO);
    glGenBuffers(1, &CVBO);

    glBindVertexArray(CVAO);
    glBindBuffer(GL_ARRAY_BUFFER, CVAO);
    glBufferData(GL_ARRAY_BUFFER, circleVertices.size() * sizeof(float), circleVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.9098039216f, 0.7490196078f, 0.1568627451f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        for (size_t i = 0; i < ringSegments.size(); i++) {
            glBindVertexArray(VAOs[i]);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, ringSegments[i].size() / 3);
        }

        glBindVertexArray(CVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, circleVertices.size() / 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(ringSegments.size(), VAOs.data());
    glDeleteBuffers(ringSegments.size(), VBOs.data());
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
