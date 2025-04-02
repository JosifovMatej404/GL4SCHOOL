// Math Constants
#define _USE_MATH_DEFINES

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>
#include <vector>

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

float hexagonRadius = 0.5f;
float rectWidth = 0.7f;
float rectHeight = 0.52f;

std::vector<float> generateHexagonVertices(float radius) {
	std::vector<float> hexagonVertices;
	for (int i = 0; i < 6; i++) {
		float angle = M_PI / 3 * i;
		hexagonVertices.push_back(radius * cos(angle));
		hexagonVertices.push_back(radius * sin(angle));
	}
	return hexagonVertices;
}

std::vector<float> generateRectangleVertices(float width, float height) {
	std::vector<float> rectVertices = {
	-width / 2, height / 2,
	width / 2, height / 2,
	width / 2, -height / 2,
	-width / 2, -height / 2
	};
	return rectVertices;
}

std::vector<GLuint> generateRectangle(float width, float height) {
	std::vector<float> rectVertices = generateRectangleVertices(width, height);
	GLuint rectVAO, rectVBO;
	glGenVertexArrays(1, &rectVAO);
	glGenBuffers(1, &rectVBO);
	glBindVertexArray(rectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
	glBufferData(GL_ARRAY_BUFFER, rectVertices.size() * sizeof(float), rectVertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	return { rectVAO, rectVBO };
}


std::vector<GLuint>  generateHexagonShape(float radius) {
	std::vector<float> hexagonVertices = generateHexagonVertices(radius);
	GLuint hexVAO, hexVBO;
	glGenVertexArrays(1, &hexVAO);
	glGenBuffers(1, &hexVBO);
	glBindVertexArray(hexVAO);
	glBindBuffer(GL_ARRAY_BUFFER, hexVBO);
	glBufferData(GL_ARRAY_BUFFER, hexagonVertices.size() * sizeof(float), hexagonVertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	return { hexVAO, hexVBO };
}

std::vector<GLuint> generateTriangles(float radius, float width, float height) {
	std::vector<float> hexagonVertices = generateHexagonVertices(radius);
	std::vector<float> rectVertices = generateRectangleVertices(width, height);
	std::vector<float> triangles = {
		rectVertices[4], rectVertices[5], hexagonVertices[0], hexagonVertices[1], rectVertices[2], rectVertices[3], // Right
		rectVertices[0], rectVertices[1], hexagonVertices[6], hexagonVertices[7], rectVertices[6], rectVertices[7], // Left
		rectVertices[6], rectVertices[7], 0, hexagonVertices[9], rectVertices[4], rectVertices[5], // Bottom 
		rectVertices[2], rectVertices[3], 0, hexagonVertices[3], rectVertices[0], rectVertices[1] // Top
	};
	GLuint triVAO, triVBO;
	glGenVertexArrays(1, &triVAO);
	glGenBuffers(1, &triVBO);
	glBindVertexArray(triVAO);
	glBindBuffer(GL_ARRAY_BUFFER, triVBO);
	glBufferData(GL_ARRAY_BUFFER, triangles.size() * sizeof(float), triangles.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	return { triVAO, triVBO };
}

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

	GLuint shaderProgram = createShaderProgram();

	std::vector<GLuint> hexVAOVBO = generateHexagonShape(hexagonRadius);
	std::vector<GLuint> rectVAOVBO = generateRectangle(rectWidth, rectHeight);
	std::vector<GLuint> triVAOVBO = generateTriangles(hexagonRadius, rectWidth, rectHeight);

    // Render Loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // Draw Triangles
		for (int i = 0; i < 3; i++) {
			
		}
		glBindVertexArray(triVAOVBO[0]);
		glDrawArrays(GL_TRIANGLES, 0, 12);

        // Draw Rectangle
        glBindVertexArray(rectVAOVBO[0]);
        glDrawArrays(GL_LINE_LOOP, 0, 4);

        // Draw Hexagon Outline
        glBindVertexArray(hexVAOVBO[0]);
        glDrawArrays(GL_LINE_LOOP, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &hexVAOVBO[0]);
    glDeleteBuffers(1, &hexVAOVBO[1]);
    glDeleteVertexArrays(1, &rectVAOVBO[0]);
    glDeleteBuffers(1, &rectVAOVBO[1]);
    glDeleteVertexArrays(1, &triVAOVBO[0]);
    glDeleteBuffers(1, &triVAOVBO[1]);
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
