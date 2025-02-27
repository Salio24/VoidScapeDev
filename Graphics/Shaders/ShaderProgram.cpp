#include "ShaderProgram.hpp"
#include <fstream>
#include <sstream>

ShaderProgram::ShaderProgram() {

}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(ID);
}

bool ShaderProgram::CreateShaderProgram(const char* vertexPath, const char* fragmentPath, const char* geometryPath) {
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        // if geometry shader path is present, also load a geometry shader
        if (geometryPath != nullptr)
        {
            gShaderFile.open(geometryPath);
            std::stringstream gShaderStream;
            gShaderStream << gShaderFile.rdbuf();
            gShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();

    const char* fShaderCode = fragmentCode.c_str();
    // 2. compile shaders
    GLuint vertex, fragment, geometry;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    if (CheckCompileErrors(vertex, "VERTEX") == false) {
		return false;
    }
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    if (CheckCompileErrors(fragment, "FRAGMENT") == false) {
        return false;
    }
    // if geometry shader is given, compile geometry shader
    if (geometryPath != nullptr)
    {
        const char* gShaderCode = geometryCode.c_str();
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);
        if (CheckCompileErrors(geometry, "GEOMETRY") == false) {
            return false;
        }
    }
    // shader Program
    ID = glCreateProgram();
    glProgramParameteri(ID, GL_PROGRAM_SEPARABLE, GL_TRUE);
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    if (geometryPath != nullptr)
        glAttachShader(ID, geometry);
    glLinkProgram(ID);
    if (CheckCompileErrors(ID, "PROGRAM") == false) {
        return false;
    }
    
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometryPath != nullptr) {
        glDeleteShader(geometry);
    }
	return true;
}

void ShaderProgram::Use() {
	glUseProgram(ID);
}

void ShaderProgram::UseInPipeline(const GLuint& PipelinePrgramID) {
    glUseProgramStages(PipelinePrgramID, GL_VERTEX_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, ID);
}

void ShaderProgram::SetBool(const std::string& name, const bool& value) const {
    glProgramUniform1i(ID, glGetUniformLocation(ID, name.c_str()), (int)value);
}

void ShaderProgram::SetInt(const std::string& name, const int& value) const {
    glProgramUniform1i(ID, glGetUniformLocation(ID, name.c_str()), value);
}

void ShaderProgram::SetFloat(const std::string& name, const float& value) const {
    glProgramUniform1f(ID, glGetUniformLocation(ID, name.c_str()), value);
}

void ShaderProgram::SetVec2(const std::string& name, const glm::vec2& value) const {
    glProgramUniform2fv(ID, glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void ShaderProgram::SetVec3(const std::string& name, const glm::vec3& value) const {
    glProgramUniform3fv(ID, glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void ShaderProgram::SetVec4(const std::string& name, const glm::vec4& value) const {
    glProgramUniform4fv(ID, glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void ShaderProgram::SetMat2(const std::string& name, const glm::mat2& value) const {
    glProgramUniformMatrix2fv(ID, glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void ShaderProgram::SetMat3(const std::string& name, const glm::mat3& value) const {
    glProgramUniformMatrix3fv(ID, glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void ShaderProgram::SetMat4(const std::string& name, const glm::mat4& value) const {
    glProgramUniformMatrix4fv(ID, glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

bool ShaderProgram::CheckCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            return false;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            return false;
        }
    }
    return true;
}
