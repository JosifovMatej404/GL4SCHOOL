#define PI 3.14159265358979323846
#include <OpenGLPrj.hpp>

#include <GLFW/glfw3.h>

#include <iostream>
#include <cmath>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;

const char* vertexShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos, 1.0);
}

)glsl";

const char* fragmentShaderSource = R"glsl(
#version 330 core
uniform vec3 outColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(outColor, 1.0);
}

)glsl";

GLuint createShaderProgram() {
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
	return shaderProgram;
}

std::vector<float> generateCircle(std::vector<float> center, float size) {
	std::vector<float> circleVertices;
	int quality = 200; // Number of segments
	for (int i = 0; i <= quality + 1; ++i) {
		float angle = 2 * PI * i / quality;
        float x, y;
        if (i % 2 == 0) {
            x = center[0] + (size - 0.2f) * cos(angle);
		    y = center[1] + (size- 0.2f) * sin(angle);
		}
        else {
            x = center[0] + size * cos(angle);
            y = center[1] + size * sin(angle);
        }
		circleVertices.push_back(x);
		circleVertices.push_back(y);
		circleVertices.push_back(0.0f); // z-coordinate
	}
	return circleVertices;
}

std::vector<float> generateBar(std::vector<float> center, float heigth, float width) {
	std::vector<float> barVertices;
	float x = center[0];
	float y = center[1];
	barVertices.push_back(x - width / 2);
	barVertices.push_back(y - heigth / 2);
	barVertices.push_back(0.0f); // z-coordinate
	barVertices.push_back(x + width / 2);
	barVertices.push_back(y - heigth / 2);
	barVertices.push_back(0.0f); // z-coordinate
	barVertices.push_back(x + width / 2);
	barVertices.push_back(y + heigth / 2);
	barVertices.push_back(0.0f); // z-coordinate
	barVertices.push_back(x - width / 2);
	barVertices.push_back(y + heigth / 2);
	barVertices.push_back(0.0f); // z-coordinate
	return barVertices;
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Color Wheel Shader", nullptr, nullptr);
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

    // Generate circle vertices
	std::vector<float> vertices = generateCircle({ 0.3f, 0.0f }, 0.5f);
    // Buffer setup
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    GLuint shaderProgram2 = createShaderProgram();

    // Generate circle vertices
    std::vector<float> vertices2 = generateBar({ -0.5f, 0.0f }, 0.8f, 0.2f);
    // Buffer setup
    GLuint VBO2, VAO2;
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);

    glBindVertexArray(VAO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);

    glBufferData(GL_ARRAY_BUFFER, vertices2.size() * sizeof(float), vertices2.data(), GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        int vertexColorLocation = glGetUniformLocation(shaderProgram, "outColor");
		glUniform3f(vertexColorLocation, 0.165f, 0.576f, 0.820);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size() / 3);

        glUseProgram(shaderProgram2);
        glBindVertexArray(VAO2);

        int vertexColorLocation2 = glGetUniformLocation(shaderProgram2, "outColor");
        glUniform3f(vertexColorLocation2, 0.192f, 0.192f, 0.514f);
		glDrawArrays(GL_TRIANGLE_FAN, 0, vertices2.size() / 3);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
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
