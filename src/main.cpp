// Math Constants
#define _USE_MATH_DEFINES

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    void main() {
        gl_Position = vec4(aPos, 0.0, 1.0);
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    void main() {
        FragColor = vec4(1.0, 0.5, 0.2, 1.0); // Orange Color
    }
)";

float hexagonRadius = 0.6f;
float rectWidth = 0.6f;
float rectHeight = 0.3f;

// Hexagon Outline (6 points)
float hexagonVertices[] = {
    hexagonRadius, 0.0f,        // Right
    hexagonRadius / 2, hexagonRadius * 0.87f, // Top Right
    -hexagonRadius / 2, hexagonRadius * 0.87f, // Top Left
    -hexagonRadius, 0.0f,       // Left
    -hexagonRadius / 2, -hexagonRadius * 0.87f, // Bottom Left
    hexagonRadius / 2, -hexagonRadius * 0.87f // Bottom Right
};

// Rectangle (4 points)
float rectVertices[] = {
    -rectWidth / 2, rectHeight / 2,  // Top Left
    rectWidth / 2, rectHeight / 2,   // Top Right
    rectWidth / 2, -rectHeight / 2,  // Bottom Right
    -rectWidth / 2, -rectHeight / 2  // Bottom Left
};

// Triangle Fill (4 triangles, 12 points)
float triangles[] = {
    hexagonVertices[2], hexagonVertices[3], rectVertices[0], rectVertices[1], hexagonVertices[0], hexagonVertices[1], // Top Right
    hexagonVertices[4], hexagonVertices[5], rectVertices[6], rectVertices[7], hexagonVertices[6], hexagonVertices[7], // Bottom Left
    hexagonVertices[0], hexagonVertices[1], rectVertices[2], rectVertices[3], hexagonVertices[8], hexagonVertices[9], // Bottom Right
    hexagonVertices[2], hexagonVertices[3], rectVertices[4], rectVertices[5], hexagonVertices[10], hexagonVertices[11] // Top Left
};

int main() {
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Hexagon with Rect", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Build and Compile Shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Setup Hexagon Outline VAO/VBO
    GLuint hexVAO, hexVBO;
    glGenVertexArrays(1, &hexVAO);
    glGenBuffers(1, &hexVBO);
    glBindVertexArray(hexVAO);
    glBindBuffer(GL_ARRAY_BUFFER, hexVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(hexagonVertices), hexagonVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Setup Rectangle VAO/VBO
    GLuint rectVAO, rectVBO;
    glGenVertexArrays(1, &rectVAO);
    glGenBuffers(1, &rectVBO);
    glBindVertexArray(rectVAO);
    glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectVertices), rectVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Setup Triangle Fill VAO/VBO
    GLuint triVAO, triVBO;
    glGenVertexArrays(1, &triVAO);
    glGenBuffers(1, &triVBO);
    glBindVertexArray(triVAO);
    glBindBuffer(GL_ARRAY_BUFFER, triVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangles), triangles, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Render Loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // Draw Triangles
        glBindVertexArray(triVAO);
        glDrawArrays(GL_TRIANGLES, 0, 12);

        // Draw Rectangle
        glBindVertexArray(rectVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        // Draw Hexagon Outline
        glBindVertexArray(hexVAO);
        glDrawArrays(GL_LINE_LOOP, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &hexVAO);
    glDeleteBuffers(1, &hexVBO);
    glDeleteVertexArrays(1, &rectVAO);
    glDeleteBuffers(1, &rectVBO);
    glDeleteVertexArrays(1, &triVAO);
    glDeleteBuffers(1, &triVBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

// Process Input
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// Resize Callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
