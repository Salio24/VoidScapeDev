#include "PipelineProgram.hpp"

PipelineProgram::PipelineProgram() {

}

PipelineProgram::~PipelineProgram() {
	glDeleteProgramPipelines(1, &ID);
}

void PipelineProgram::CreatePipelineProgram() {
	glGenProgramPipelines(1, &ID);
}

void PipelineProgram::BindPipelineProgram() {
	glBindProgramPipeline(ID);
}

void PipelineProgram::BindShaderProgram(const GLuint& shaderProgramID) {
	glUseProgramStages(ID, GL_VERTEX_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, shaderProgramID);
}