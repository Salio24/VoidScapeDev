#include "BatchRenderer.hpp"

BatchRenderer::BatchRenderer() {

}

BatchRenderer::~BatchRenderer() {

}
void BatchRenderer::StartUp(ShaderProgram* program, GLuint& PipelineProgramID) {
	if (QuadBuffer != nullptr) {
		std::cout << "BatchRenderer has been initialized twice. ERROR!" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	currentProgram = program;
	currentPipelineProgramID = PipelineProgramID;

	QuadBuffer = new Box[maxVertexCount];

	glCreateVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glCreateBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, maxVertexCount * sizeof(Box), nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexArrayAttrib(VAO, 0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(Box), (const void*)offsetof(Box, Position));

	glEnableVertexArrayAttrib(VAO, 1);
	glVertexAttribPointer(1, 4, GL_FLOAT, false, sizeof(Box), (const void*)offsetof(Box, Color));

	glEnableVertexArrayAttrib(VAO, 2);
	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Box), (const void*)offsetof(Box, TexturePosition));

	glEnableVertexArrayAttrib(VAO, 3);
	glVertexAttribPointer(3, 1, GL_FLOAT, false, sizeof(Box), (const void*)offsetof(Box, TextureIndex));

	glEnableVertexArrayAttrib(VAO, 4);
	glVertexAttribPointer(4, 1, GL_FLOAT, false, sizeof(Box), (const void*)offsetof(Box, RotationalAngle));

	glEnableVertexArrayAttrib(VAO, 5);
	glVertexAttribPointer(5, 1, GL_FLOAT, false, sizeof(Box), (const void*)offsetof(Box, AngleModifier));

	glEnableVertexArrayAttrib(VAO, 6);
	glVertexAttribPointer(6, 2, GL_FLOAT, false, sizeof(Box), (const void*)offsetof(Box, TranslateMargin));

	uint32_t indices[maxIndexCount];
	uint32_t offset = 0;
	for (int i = 0; i < maxIndexCount; i += 6) {
		indices[i + 0] = 0 + offset;
		indices[i + 1] = 1 + offset;
		indices[i + 2] = 2 + offset;

		indices[i + 3] = 2 + offset;
		indices[i + 4] = 3 + offset;
		indices[i + 5] = 0 + offset;

		offset += 4;
	}
	glCreateBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void BatchRenderer::ShutDown() {
	glDeleteVertexArrays(1, &VAO); 
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);

	delete[] QuadBuffer;
}

void BatchRenderer::BeginBatch(const glm::mat4& ProjectionMatrix, const glm::mat4* modelMatrix, int slot) {
	QuadBufferPtr = QuadBuffer;
	currentProjectionMatrix = ProjectionMatrix;
	currentModelMatrix = modelMatrix;
	currentTextureSlot = slot;
}

void BatchRenderer::EndBatch() {
	GLsizeiptr size = (uint8_t*)QuadBufferPtr - (uint8_t*)QuadBuffer;
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, QuadBuffer);
}

void BatchRenderer::DrawInBatch(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
	if (indexCount >= maxIndexCount) {
		EndBatch();
		Flush();
		BeginBatch(currentProjectionMatrix, currentModelMatrix);
	}

	QuadBufferPtr->Position = {position.x, position.y};
	QuadBufferPtr->Color = color;
	QuadBufferPtr->TexturePosition = { 0.0f, 0.0f };
	QuadBufferPtr->TextureIndex = 0;
	QuadBufferPtr->RotationalAngle = 0;
	QuadBufferPtr++;

	QuadBufferPtr->Position = { position.x + size.x, position.y };
	QuadBufferPtr->Color = color;
	QuadBufferPtr->TexturePosition = { 0.03125f, 0.0f };
	QuadBufferPtr->TextureIndex = 0;
	QuadBufferPtr->RotationalAngle = 0;
	QuadBufferPtr++;

	QuadBufferPtr->Position = { position.x + size.x, position.y + size.y};
	QuadBufferPtr->Color = color;
	QuadBufferPtr->TexturePosition = { 0.03125f, 0.03125f };
	QuadBufferPtr->TextureIndex = 0;
	QuadBufferPtr->RotationalAngle = 0;
	QuadBufferPtr++;

	QuadBufferPtr->Position = { position.x, position.y + size.y };
	QuadBufferPtr->Color = color;
	QuadBufferPtr->TexturePosition = { 0.0f, 0.03125f };
	QuadBufferPtr->TextureIndex = 0;
	QuadBufferPtr->RotationalAngle = 0;
	QuadBufferPtr++;

	indexCount += 6;
}

void BatchRenderer::DrawInBatch(const glm::vec2 position, const glm::vec2 size, uint32_t textureID, const glm::vec2 textureSize, const glm::vec2 texturePosition, const float rotationAnlge, const float anlgeModifier, const bool drawFliped, const glm::vec4& color) {
	if (indexCount >= maxIndexCount) {
		EndBatch();
		Flush();
		BeginBatch(currentProjectionMatrix, currentModelMatrix, currentTextureSlot);
	}

	if (drawFliped == true) {
		if (rotationAnlge == 0.0f) {
			QuadBufferPtr->Position = { position.x, position.y };
			QuadBufferPtr->Color = color;
			QuadBufferPtr->TexturePosition = { texturePosition.x + textureSize.x, texturePosition.y };
			QuadBufferPtr->TextureIndex = textureID;
			QuadBufferPtr->RotationalAngle = rotationAnlge;
			QuadBufferPtr++;

			QuadBufferPtr->Position = { position.x + size.x, position.y };
			QuadBufferPtr->Color = color;
			QuadBufferPtr->TexturePosition = { texturePosition.x, texturePosition.y };
			QuadBufferPtr->TextureIndex = textureID;
			QuadBufferPtr->RotationalAngle = rotationAnlge;
			QuadBufferPtr++;

			QuadBufferPtr->Position = { position.x + size.x, position.y + size.y };
			QuadBufferPtr->Color = color;
			QuadBufferPtr->TexturePosition = { texturePosition.x, texturePosition.y + textureSize.y };
			QuadBufferPtr->TextureIndex = textureID;
			QuadBufferPtr->RotationalAngle = rotationAnlge;
			QuadBufferPtr++;

			QuadBufferPtr->Position = { position.x, position.y + size.y };
			QuadBufferPtr->Color = color;
			QuadBufferPtr->TexturePosition = { texturePosition.x + textureSize.x, texturePosition.y + textureSize.y };
			QuadBufferPtr->TextureIndex = textureID;
			QuadBufferPtr->RotationalAngle = rotationAnlge;
			QuadBufferPtr++;
		}
		else {
			QuadBufferPtr->Position = { position.x, position.y };
			QuadBufferPtr->Color = color;
			QuadBufferPtr->TexturePosition = { texturePosition.x + textureSize.x, texturePosition.y };
			QuadBufferPtr->TextureIndex = textureID;
			QuadBufferPtr->RotationalAngle = rotationAnlge;
			QuadBufferPtr->AngleModifier = anlgeModifier;
			QuadBufferPtr->TranslateMargin = { position.x + size.x / 2.0f, position.y + size.y / 2.0f };
			QuadBufferPtr++;

			QuadBufferPtr->Position = { position.x + size.x, position.y };
			QuadBufferPtr->Color = color;
			QuadBufferPtr->TexturePosition = { texturePosition.x, texturePosition.y };
			QuadBufferPtr->TextureIndex = textureID;
			QuadBufferPtr->RotationalAngle = rotationAnlge;
			QuadBufferPtr->AngleModifier = anlgeModifier;
			QuadBufferPtr->TranslateMargin = { position.x + size.x / 2.0f, position.y + size.y / 2.0f };
			QuadBufferPtr++;

			QuadBufferPtr->Position = { position.x + size.x, position.y + size.y };
			QuadBufferPtr->Color = color;
			QuadBufferPtr->TexturePosition = { texturePosition.x, texturePosition.y + textureSize.y };
			QuadBufferPtr->TextureIndex = textureID;
			QuadBufferPtr->RotationalAngle = rotationAnlge;
			QuadBufferPtr->AngleModifier = anlgeModifier;
			QuadBufferPtr->TranslateMargin = { position.x + size.x / 2.0f, position.y + size.y / 2.0f };
			QuadBufferPtr++;

			QuadBufferPtr->Position = { position.x, position.y + size.y };
			QuadBufferPtr->Color = color;
			QuadBufferPtr->TexturePosition = { texturePosition.x + textureSize.x, texturePosition.y + textureSize.y };
			QuadBufferPtr->TextureIndex = textureID;
			QuadBufferPtr->RotationalAngle = rotationAnlge;
			QuadBufferPtr->AngleModifier = anlgeModifier;
			QuadBufferPtr->TranslateMargin = { position.x + size.x / 2.0f, position.y + size.y / 2.0f };
			QuadBufferPtr++;
		}
	}
	else {
		if (rotationAnlge == 0.0f) {
			QuadBufferPtr->Position = { position.x, position.y };
			QuadBufferPtr->Color = color;
			QuadBufferPtr->TexturePosition = { texturePosition.x, texturePosition.y };
			QuadBufferPtr->TextureIndex = textureID;
			QuadBufferPtr->RotationalAngle = rotationAnlge;
			QuadBufferPtr++;

			QuadBufferPtr->Position = { position.x + size.x, position.y };
			QuadBufferPtr->Color = color;
			QuadBufferPtr->TexturePosition = { texturePosition.x + textureSize.x, texturePosition.y };
			QuadBufferPtr->TextureIndex = textureID;
			QuadBufferPtr->RotationalAngle = rotationAnlge;
			QuadBufferPtr++;

			QuadBufferPtr->Position = { position.x + size.x, position.y + size.y };
			QuadBufferPtr->Color = color;
			QuadBufferPtr->TexturePosition = { texturePosition.x + textureSize.x, texturePosition.y + textureSize.y };
			QuadBufferPtr->TextureIndex = textureID;
			QuadBufferPtr->RotationalAngle = rotationAnlge;
			QuadBufferPtr++;

			QuadBufferPtr->Position = { position.x, position.y + size.y };
			QuadBufferPtr->Color = color;
			QuadBufferPtr->TexturePosition = { texturePosition.x, texturePosition.y + textureSize.y };
			QuadBufferPtr->TextureIndex = textureID;
			QuadBufferPtr->RotationalAngle = rotationAnlge;
			QuadBufferPtr++;
		}
		else {
			QuadBufferPtr->Position = { position.x, position.y };
			QuadBufferPtr->Color = color;
			QuadBufferPtr->TexturePosition = { texturePosition.x, texturePosition.y };
			QuadBufferPtr->TextureIndex = textureID;
			QuadBufferPtr->RotationalAngle = rotationAnlge;
			QuadBufferPtr->AngleModifier = anlgeModifier;
			QuadBufferPtr->TranslateMargin = { position.x + size.x / 2.0f, position.y + size.y / 2.0f };
			QuadBufferPtr++;

			QuadBufferPtr->Position = { position.x + size.x, position.y };
			QuadBufferPtr->Color = color;
			QuadBufferPtr->TexturePosition = { texturePosition.x + textureSize.x, texturePosition.y };
			QuadBufferPtr->TextureIndex = textureID;
			QuadBufferPtr->RotationalAngle = rotationAnlge;
			QuadBufferPtr->AngleModifier = anlgeModifier;
			QuadBufferPtr->TranslateMargin = { position.x + size.x / 2.0f, position.y + size.y / 2.0f };
			QuadBufferPtr++;

			QuadBufferPtr->Position = { position.x + size.x, position.y + size.y };
			QuadBufferPtr->Color = color;
			QuadBufferPtr->TexturePosition = { texturePosition.x + textureSize.x, texturePosition.y + textureSize.y };
			QuadBufferPtr->TextureIndex = textureID;
			QuadBufferPtr->RotationalAngle = rotationAnlge;
			QuadBufferPtr->AngleModifier = anlgeModifier;
			QuadBufferPtr->TranslateMargin = { position.x + size.x / 2.0f, position.y + size.y / 2.0f };
			QuadBufferPtr++;

			QuadBufferPtr->Position = { position.x, position.y + size.y };
			QuadBufferPtr->Color = color;
			QuadBufferPtr->TexturePosition = { texturePosition.x, texturePosition.y + textureSize.y };
			QuadBufferPtr->TextureIndex = textureID;
			QuadBufferPtr->RotationalAngle = rotationAnlge;
			QuadBufferPtr->AngleModifier = anlgeModifier;
			QuadBufferPtr->TranslateMargin = { position.x + size.x / 2.0f, position.y + size.y / 2.0f };
			QuadBufferPtr++;
		}
	}

	indexCount += 6;
}


void BatchRenderer::DrawSeperatly(const glm::vec2& position, glm::vec2 size, const glm::vec4& color, const glm::mat4& ProjectionMatrix, const glm::mat4* ModelMatrix) {
	BeginBatch(ProjectionMatrix, ModelMatrix);
	DrawInBatch(position, size, color);
	EndBatch();
	Flush();
}

void BatchRenderer::DrawSeperatly(const glm::mat4& ProjectionMatrix, const glm::vec2 position, glm::vec2 size, uint32_t textureID, const glm::vec2 textureSize, const glm::vec2 texturePosition, const float rotationAnlge, const float anlgeModifier, const bool drawFliped, const glm::mat4* ModelMatrix, const glm::vec4& color) {
	BeginBatch(ProjectionMatrix, ModelMatrix);
	DrawInBatch(position, size, textureID, textureSize, texturePosition, rotationAnlge, anlgeModifier, drawFliped, color);
	EndBatch();
	Flush();
}

void BatchRenderer::Flush() {
	currentProgram->UseInPipeline(currentPipelineProgramID);
	currentProgram->SetMat4("uProjectionMatrix", currentProjectionMatrix);
	if (currentModelMatrix != nullptr) {
		currentProgram->SetMat4("uModelMatrix", *currentModelMatrix);
	}
	else {
		currentProgram->SetMat4("uModelMatrix", glm::mat4(1.0f));
	}
	currentProgram->SetInt("uTextures", currentTextureSlot);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
	indexCount = 0;
}