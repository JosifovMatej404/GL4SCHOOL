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
out vec2 fragPos;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    fragPos = aPos.xy;
}

)glsl";

const char* fragmentShaderSource = R"glsl(
#version 330 core

in vec2 fragPos;
uniform vec3 startColor;
uniform vec3 endColor;

out vec4 FragColor;

void main()
{
    float t = (fragPos.x + 1.0) / 2.0;
    vec3 gradientColor = mix(startColor, endColor, t);
    FragColor = vec4(gradientColor, 1.0);
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

std::vector<std::vector<float>> generateBars(std::vector<float> startingPosition, std::vector<float> offset,
                                            float heigth, float width, int count) {
    std::vector<std::vector<float>> bars;
    for (int i = 0; i < count; i++) {
		std::vector<float> barVertices;
		float x = startingPosition[0] + offset[0] * i;
		float y = startingPosition[1] + offset[1] * i;
		barVertices.push_back(x - width / 2);
		barVertices.push_back(y - heigth / 2);
		barVertices.push_back(0.0f);
		barVertices.push_back(x + width / 2);
		barVertices.push_back(y - heigth / 2);
		barVertices.push_back(0.0f);
		barVertices.push_back(x + width / 2);
		barVertices.push_back(y + heigth / 2);
		barVertices.push_back(0.0f);
		barVertices.push_back(x - width / 2);
		barVertices.push_back(y + heigth / 2);
		barVertices.push_back(0.0f);
		bars.push_back(barVertices);
    }
	return bars;
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

    std::vector<std::vector<float>> bars = generateBars({ 0.0f, 0.6f }, { 0.0f, -0.3f }, 0.2f, 1.0f, 5);
   
    std::vector<GLuint> shaderPrograms;

    for (int i = 0; i < bars.size(); i++) {
        shaderPrograms.push_back(createShaderProgram());
    }

    // Buffer setup
    std::vector<GLuint> VBOs(bars.size()), VAOs(bars.size());
    glGenVertexArrays(bars.size(), VBOs.data());
    glGenBuffers(bars.size(), VAOs.data());

	for (int i = 0; i < bars.size(); i++) {
		glBindVertexArray(VAOs[i]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
		glBufferData(GL_ARRAY_BUFFER, bars[i].size() * sizeof(float), bars[i].data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	std::vector<float> colors = {
        0.0f, 0.0f, 0.65f, 1.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 1.0f, 0.4f, 0.0f,
        0.6f, 0.0f, 0.0f, 0.0f, 0.8f, 0.0f,
		0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.5f, 0.5f, 1.0f, 0.5f, 0.0f
	};

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        for (int i = 0; i < bars.size(); i++) {
			glUseProgram(shaderPrograms[i]);
			glBindVertexArray(VAOs[i]);
			int vertexColorLocation = glGetUniformLocation(shaderPrograms[i], "startColor");
			glUniform3f(vertexColorLocation, colors[i*6], colors[i*6 + 1], colors[i*6 + 2]);
            int vertexColorLocation2 = glGetUniformLocation(shaderPrograms[i], "endColor");
            glUniform3f(vertexColorLocation2, colors[i*6+3], colors[i*6 + 4], colors[i*6 + 5]);
			glDrawArrays(GL_TRIANGLE_FAN, 0, bars[i].size() / 3);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
	for (int i = 0; i < bars.size(); i++) {
		glDeleteVertexArrays(1, &VAOs[i]);
		glDeleteBuffers(1, &VBOs[i]);
		glDeleteProgram(shaderPrograms[i]);
	}
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
