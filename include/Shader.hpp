#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/glad.h> // include glad to get all the required OpenGL headers
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    enum SHADER_TYPE {VERTEX, FRAGMENT, GEOMETRY};

    unsigned int ID;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char* vertexPath, const char* fragmentPath);

    // constructor using std::string
    // ------------------------------------------------------------------------
    Shader(const std::string vertexPath, const std::string fragmentPath);

    // activate the shader
    // ------------------------------------------------------------------------
    void use();

    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const;

    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const;

    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const;

    void setMat4(const std::string& name, const glm::mat4& mat) const;

    void setVec3(const std::string& name, const glm::vec3& value) const;

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type);

    void readShader(char const * const, Shader::SHADER_TYPE);

    void compileShader();


    std::string vertexShader;
    std::string fragmentShader;

};



#endif // SHADER_HPP
