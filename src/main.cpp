#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <OpenGLPrj.hpp>
#include <Shader.hpp>

// Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Plane VAO
unsigned int planeVAO = 0, planeVBO, planeEBO;

void createPlane()
{
    float planeVertices[] = {
        // positions          // normals         // texcoords // tangents
        -2.0f, 0.0f, -2.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
         2.0f, 0.0f, -2.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
         2.0f, 0.0f,  2.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,  1.0f, 0.0f, 0.0f,

         2.0f, 0.0f,  2.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
        -2.0f, 0.0f,  2.0f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
        -2.0f, 0.0f, -2.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
    };

    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);

    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // TexCoords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // Tangent
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
}



unsigned int loadTexture(const char* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);

    if (data)
    {
        GLenum format = GL_RGB;
        if (nrComponents == 1) format = GL_RED;
        else if (nrComponents == 4) format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cout << "Failed to load texture: " << path << std::endl;
    }
    stbi_image_free(data);
    return textureID;
}

int main()
{
    // GLFW init
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "PBR Plane", NULL, NULL);
    if (!window) return -1;
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glEnable(GL_DEPTH_TEST);

    Shader pbrShader("../res/shaders/shader.vert", "../res/shaders/shader.frag");



    createPlane();
        
    // Load PBR Textures
    unsigned int albedoMap = loadTexture("../res/textures/base.png");
    unsigned int normalMap = loadTexture("../res/textures/normal.png");
    unsigned int metallicMap = loadTexture("../res/textures/metallic.png");
    unsigned int roughnessMap = loadTexture("../res/textures/roughness.png");
    unsigned int aoMap = loadTexture("../res/textures/ambient.png");
    unsigned int heightMap = loadTexture("../res/textures/height.png");

    // Bind texture units
    pbrShader.use();
    pbrShader.setInt("albedoMap", 0);
    pbrShader.setInt("normalMap", 1);
    pbrShader.setInt("metallicMap", 2);
    pbrShader.setInt("roughnessMap", 3);
    pbrShader.setInt("aoMap", 4);
    pbrShader.setInt("heightMap", 5);

    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

    while (!glfwWindowShouldClose(window))
    {
        float time = glfwGetTime();
        float deltaTime = 0.016f;

        glm::vec3 lightPos = glm::vec3(sin(time) * 3.0f, 1.5f + sin(time * 0.5f) * 0.5f, cos(time) * 3.0f);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(glm::vec3(0.0, 1.0, 5.0), glm::vec3(0.0), glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 model = glm::mat4(1.0f);

        // render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        pbrShader.use();


        pbrShader.setMat4("projection", projection);
        pbrShader.setMat4("view", view);
        pbrShader.setMat4("model", model);
        pbrShader.setVec3("camPos", glm::vec3(0.0, 1.0, 5.0));
        pbrShader.setVec3("lightPos", lightPos);
        pbrShader.setVec3("lightColor", lightColor);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, albedoMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalMap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, metallicMap);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, roughnessMap);

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, aoMap);

        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, heightMap);

        glBindVertexArray(planeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
