#pragma once
#include <glad/glad.h>

class PipelineProgram {
public:
	PipelineProgram();

	~PipelineProgram();

	void CreatePipelineProgram();

	void BindPipelineProgram();

	void BindShaderProgram(const GLuint& shaderProgramID);

	GLuint ID;
};