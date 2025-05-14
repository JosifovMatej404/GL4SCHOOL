// main.cpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// Vertex Shader
const char* vertexShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec2 aPos;
out vec2 fragCoord;
void main()
{
    fragCoord = aPos;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)glsl";

// Fragment Shader
const char* fragmentShaderSource = R"glsl(
#version 330 core
in vec2 fragCoord;
out vec4 FragColor;

// Color palette
vec3 red = vec3(0.5, 0.0, 0.0);
vec3 green = vec3(0.0, 0.5, 0.0);
vec3 black = vec3(0.0);
vec3 white = vec3(1.0);
vec3 ivory = vec3(1.0, 1.0, 0.9);

// Number of sectors and rings
const int NUM_SECTORS = 20;

void main()
{
    vec2 uv = fragCoord;
    float r = length(uv);
    float angle = atan(uv.y, uv.x);
    if (angle < 0.0) angle += 6.28318;

    // Normalize angle to 0..1
    float sector = angle / 6.28318 * float(NUM_SECTORS);
    int iSector = int(floor(sector));

    float band = r * 10.0;
    int iBand = int(floor(band));

    vec3 color = ivory;

    if (r > 1.0) discard;

    if (iBand == 9 || iBand == 6) {
        // Thin outer and inner rings
        color = (iSector % 2 == 0) ? green : red;
    } else if (iBand >= 7) {
        color = (iSector % 2 == 0) ? ivory : black;
    } else if (iBand >= 3) {
        color = (iSector % 2 == 0) ? ivory : black;
    } else if (iBand >= 1) {
        color = green;
    } else {
        color = red;
    }

    FragColor = vec4(color, 1.0);
}
)glsl";

float quadVertices[] = {
    -1.0f, -1.0f,
     1.0f, -1.0f,
    -1.0f,  1.0f,
     1.0f,  1.0f,
};

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Dartboard Shader", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // Compile vertex shader
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexShaderSource, NULL);
    glCompileShader(vs);

    // Compile fragment shader
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentShaderSource, NULL);
    glCompileShader(fs);

    // Link program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);

    glDeleteShader(vs);
    glDeleteShader(fs);

    // Set up fullscreen quad
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}