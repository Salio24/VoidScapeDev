#pragma once
#include <ft2build.h>
#include <iostream>
#include <glm/glm.hpp>
#include <map>
#include <glad/glad.h>
#include "ShaderProgram.hpp"
#include <glm/ext/matrix_transform.hpp>
#include FT_FREETYPE_H

struct Character {
	unsigned int TextureID;  // ID handle of the glyph texture
	glm::ivec2   Size;       // Size of glyph
	glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
	unsigned int Advance;    // Offset to advance to next glyph
};



class TextRenderer {
public:
	TextRenderer();

	~TextRenderer();

	void Init(const char* filepath, const int& fontSize = 48);

	void SetFontSize(const int& fontSize);

	void TextRenderer::RenderText(ShaderProgram* shader, GLuint& pipelineProgramID, std::string text, float x, float y, float scale, glm::vec3 color, glm::mat4 projectionMatrix, glm::mat4 modelMatrix = glm::mat4(1.0f), bool centeredX = true, bool centeredY = true, bool rightSided = false);

	FT_Library ft;

	std::map<char, Character> Characters;

	FT_Face face;

private:
	GLuint VAO{ 0 };
	GLuint VBO{ 0 };
	GLuint IBO{ 0 };

	glm::mat4 currentProjectionMatrix;
	ShaderProgram* currentProgram;
	GLuint currentPipelineProgramID;

	int sizeMultiplier{ 4 };
};