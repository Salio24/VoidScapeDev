#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "glm/gtx/string_cast.hpp"
#include "../Graphics/Shaders/ShaderProgram.hpp"
#include "../Utilities/HelperStructs.hpp"
#include <cstddef>

class App;

class BatchRenderer {
public:
	BatchRenderer();

	~BatchRenderer();

	void StartUp(ShaderProgram* program, GLuint& PipelineProgramID);
	void ShutDown();

	void BeginBatch(const glm::mat4& ProjectionMatrix, const glm::mat4* modelMatrix = nullptr, int slot = 0);
	void EndBatch();
	void Flush();

	void DrawSeperatly(const glm::vec2& position, glm::vec2 size, const glm::vec4& color, const glm::mat4& ProjectionMatrix, const glm::mat4* ModelMatrix = nullptr);
	void DrawSeperatly(const glm::mat4& ProjectionMatrix, const glm::vec2 position, glm::vec2 size, uint32_t textureID, const glm::vec2 textureSize, const glm::vec2 texturePosition = glm::vec2(0.0f, 0.0f), const float rotationAnlge = 0.0f, const float anlgeModifier = 1.0f, const bool drawFliped = false, const glm::mat4* ModelMatrix = nullptr, const glm::vec4& color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	void DrawInBatch(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);

	void DrawInBatch(const glm::vec2 position, const glm::vec2 size, uint32_t textureID, const glm::vec2 textureSize, const glm::vec2 texturePosition = glm::vec2(0.0f, 0.0f), const float rotationAnlge = 0.0f, const float anlgeModifier = 1.0f, const bool drawFliped = false, const glm::vec4& color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

private:
	App* app_;

	App& app();

	uint32_t indexCount{ 0 };

	Box* QuadBuffer{ nullptr };
	Box* QuadBufferPtr{ nullptr };

	glm::mat4 currentProjectionMatrix;
	const glm::mat4* currentModelMatrix;


	ShaderProgram* currentProgram;
	GLuint currentPipelineProgramID;
	int currentTextureSlot;

	GLuint VAO{ 0 };
	GLuint VBO{ 0 };
	GLuint IBO{ 0 };

	static const size_t maxQuadCount{ 1000 };
	static const size_t maxVertexCount{ maxQuadCount * 4 };
	static const size_t maxIndexCount{ maxQuadCount * 6 };
};