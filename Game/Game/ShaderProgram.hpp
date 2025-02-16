#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class ShaderProgram {
public:
	ShaderProgram();

	~ShaderProgram();

	bool CreateShaderProgram(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);

	void Use();

	void UseInPipeline(const GLuint& PipelinePrgramID);

	void SetBool(const std::string &name, const bool& value) const;
	void SetInt(const std::string& name, const int& value) const;
	void SetFloat(const std::string& name, const float& value) const;
	void SetVec2(const std::string& name, const glm::vec2& value) const;
	void SetVec3(const std::string& name, const glm::vec3& value) const;
	void SetVec4(const std::string& name, const glm::vec4& value) const;
	void SetMat2(const std::string& name, const glm::mat2& value) const;
	void SetMat3(const std::string& name, const glm::mat3& value) const;
	void SetMat4(const std::string& name, const glm::mat4& value) const;

	GLuint ID;
private:
	bool CheckCompileErrors(GLuint shader, std::string type);
};