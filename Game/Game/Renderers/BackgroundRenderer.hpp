#pragma once
#include <glad/glad.h>
#include "../Utilities/HelperStructs.hpp"
#include "../Graphics/Shaders/ShaderProgram.hpp"
#include <SDL3/SDL.h>
#include "../Graphics/Texture.hpp"

struct LevelBackgroundLayer {
	Box b1;
	Box b2;
	Box b3;

	int offset;

	float speed_modifier;

	uint32_t textureID;
};

class BackgroundRenderer {
public:
	BackgroundRenderer();

	~BackgroundRenderer();

	void Init();

	void RenderMenuBackground(ShaderProgram* shader, GLuint& pipelineProgramID, const glm::mat4& model, const glm::mat4& projection);

	void LoadMenuBackground(const char* filepath, ShaderProgram* shader, GLuint& pipelineProgramID, TextureHandler* textureHandler);

	void LoadLevelBackground(ShaderProgram* shader, ShaderProgram* framebufferShader, GLuint& pipelineProgramID, TextureHandler* textureHandler);
	
	void RenderLevelBackground(ShaderProgram* shader, ShaderProgram* framebufferShader, GLuint& pipelineProgramID, const glm::mat4& projection, const float& blackHoleAABBx, glm::vec2 blackHoleCenterPos, const float& camPosx, bool gameStarted, const float& delta);

	void Update(const glm::vec2& camPos, const glm::vec2& actorVelocity, const float& deltaTime);

	void SetDefaultLayerValues(LevelBackgroundLayer& layer);

	void LayerPositionUpdate(LevelBackgroundLayer& layer, const glm::vec2& camPos, const glm::vec2& actorVelocity, const float& deltaTime);

	void ResizeFramebuffer(glm::vec2 resolution);

	std::vector<Box> GetBoxes(LevelBackgroundLayer& layer);

private:

	LevelBackgroundLayer layer0;
	LevelBackgroundLayer layer1;
	LevelBackgroundLayer layer2;
	LevelBackgroundLayer layer3;
	LevelBackgroundLayer layer4_lights;
	LevelBackgroundLayer layer5;
	LevelBackgroundLayer layer6;
	LevelBackgroundLayer layer7_lights;
	LevelBackgroundLayer layer8;
	LevelBackgroundLayer layer9;
	LevelBackgroundLayer layer10;
	LevelBackgroundLayer layer11;

	glm::vec2 backgroundSize = glm::vec2((1080.0f / 793.0f) * 928.0f, 1080.0f);
	//glm::vec2 backgroundSize = glm::vec2(1262.0f, 1080.0f);

	//x	1263.85876	float


	int temp{0};

	GLuint VAO;
	GLuint VBO;
	GLuint IBO;

	GLuint FBO;
	GLuint RBO;
	GLuint screenTexture;
	GLuint framebufferVAO;
	GLuint framebufferVBO;

	float time;
	float delt;

	GLuint texture;

	glm::vec2 currentResolution{1920, 1080};

	GLuint textureArray;
};