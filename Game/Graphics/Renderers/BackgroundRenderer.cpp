#include "BackgroundRenderer.hpp"

BackgroundRenderer::BackgroundRenderer() {

}

BackgroundRenderer::~BackgroundRenderer() {

}

void BackgroundRenderer::Init() {
	glCreateVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glCreateBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 216 * sizeof(Box), nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexArrayAttrib(VAO, 0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(Box), (const void*)offsetof(Box, Position));

	glEnableVertexArrayAttrib(VAO, 1);
	glVertexAttribPointer(1, 4, GL_FLOAT, false, sizeof(Box), (const void*)offsetof(Box, Color));

	glEnableVertexArrayAttrib(VAO, 2);
	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Box), (const void*)offsetof(Box, TexturePosition));

	glEnableVertexArrayAttrib(VAO, 3);
	glVertexAttribPointer(3, 1, GL_FLOAT, false, sizeof(Box), (const void*)offsetof(Box, TextureIndex));

	uint32_t indices[216];
	uint32_t offset = 0;
	for (int i = 0; i < 216; i += 6) {
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

	glBindVertexArray(0);
}

void BackgroundRenderer::RenderMenuBackground(ShaderProgram* shader, GLuint& pipelineProgramID, const glm::mat4& model, const glm::mat4& projection) {
	shader->UseInPipeline(pipelineProgramID);

	//glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	std::vector<Box> boxes;
	
	{
		Box b;
		b.Position = glm::vec2(0.0f, 0.0f);
		b.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		b.TexturePosition = glm::vec2(0.0f, 0.0f);
		boxes.push_back(b);
	} {
		Box b;
		b.Position = glm::vec2(1920.0f, 0.0f);
		b.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		b.TexturePosition = glm::vec2(1.0f, 0.0f);
		boxes.push_back(b);
	} {
		Box b;
		b.Position = glm::vec2(1920.0f, 1080.0f);
		b.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		b.TexturePosition = glm::vec2(1.0f, 1.0f);
		boxes.push_back(b);
	} {
		Box b;
		b.Position = glm::vec2(0.0f, 1080.0f);
		b.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		b.TexturePosition = glm::vec2(0.0f, 1.0f);
		boxes.push_back(b);
	}

	glBufferSubData(GL_ARRAY_BUFFER, 0, boxes.size() * sizeof(Box), boxes.data());

	shader->SetMat4("uModelMatrix", model);
	shader->SetMat4("uProjectionMatrix", projection);
	shader->SetInt("uLevelBackground", 0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void BackgroundRenderer::LoadMenuBackground(const char* filepath, ShaderProgram* shader, GLuint& pipelineProgramID, TextureHandler* textureHandler) {
	shader->UseInPipeline(pipelineProgramID);

	SDL_Surface* surface = textureHandler->FlipSurfaceVertically(textureHandler->LoadSurface(filepath));

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void BackgroundRenderer::LoadLevelBackground(ShaderProgram* shader, ShaderProgram* framebufferShader, GLuint& pipelineProgramID, TextureHandler* textureHandler) {
	shader->UseInPipeline(pipelineProgramID);
	shader->SetInt("uTextureArray", 1);

	SDL_Surface* layer0Surface = textureHandler->FlipSurfaceVertically(textureHandler->LoadSurface("assets/UI/Background_layers/Layer_0000_9.png"));
	SDL_Surface* layer1Surface = textureHandler->FlipSurfaceVertically(textureHandler->LoadSurface("assets/UI/Background_layers/Layer_0001_8.png"));
	SDL_Surface* layer2Surface = textureHandler->FlipSurfaceVertically(textureHandler->LoadSurface("assets/UI/Background_layers/Layer_0002_7.png"));
	SDL_Surface* layer3Surface = textureHandler->FlipSurfaceVertically(textureHandler->LoadSurface("assets/UI/Background_layers/Layer_0003_6.png"));
	SDL_Surface* layer4_lightsSurface = textureHandler->FlipSurfaceVertically(textureHandler->LoadSurface("assets/UI/Background_layers/Layer_0004_Lights.png"));
	SDL_Surface* layer5Surface = textureHandler->FlipSurfaceVertically(textureHandler->LoadSurface("assets/UI/Background_layers/Layer_0005_5.png"));
	SDL_Surface* layer6Surface = textureHandler->FlipSurfaceVertically(textureHandler->LoadSurface("assets/UI/Background_layers/Layer_0006_4.png"));
	SDL_Surface* layer7_lightsSurface = textureHandler->FlipSurfaceVertically(textureHandler->LoadSurface("assets/UI/Background_layers/Layer_0007_Lights.png"));
	SDL_Surface* layer8Surface = textureHandler->FlipSurfaceVertically(textureHandler->LoadSurface("assets/UI/Background_layers/Layer_0008_3.png"));
	SDL_Surface* layer9Surface = textureHandler->FlipSurfaceVertically(textureHandler->LoadSurface("assets/UI/Background_layers/Layer_0009_2.png"));
	SDL_Surface* layer10Surface = textureHandler->FlipSurfaceVertically(textureHandler->LoadSurface("assets/UI/Background_layers/Layer_0010_1.png"));
	SDL_Surface* layer11Surface = textureHandler->FlipSurfaceVertically(textureHandler->LoadSurface("assets/UI/Background_layers/Layer_0011_0.png"));
	glActiveTexture(GL_TEXTURE30);

	glGenTextures(1, &textureArray);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, layer0Surface->w, layer0Surface->h, 12);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, layer0Surface->w, layer0Surface->h, 1, GL_RGBA, GL_UNSIGNED_BYTE, layer0Surface->pixels);
	SDL_DestroySurface(layer0Surface);

	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 1, layer1Surface->w, layer1Surface->h, 1, GL_RGBA, GL_UNSIGNED_BYTE, layer1Surface->pixels);
	SDL_DestroySurface(layer1Surface);

	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 2, layer2Surface->w, layer2Surface->h, 1, GL_RGBA, GL_UNSIGNED_BYTE, layer2Surface->pixels);
	SDL_DestroySurface(layer2Surface);

	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 3, layer3Surface->w, layer3Surface->h, 1, GL_RGBA, GL_UNSIGNED_BYTE, layer3Surface->pixels);
	SDL_DestroySurface(layer3Surface);

	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 4, layer4_lightsSurface->w, layer4_lightsSurface->h, 1, GL_RGBA, GL_UNSIGNED_BYTE, layer4_lightsSurface->pixels);
	SDL_DestroySurface(layer4_lightsSurface);

	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 5, layer5Surface->w, layer5Surface->h, 1, GL_RGBA, GL_UNSIGNED_BYTE, layer5Surface->pixels);
	SDL_DestroySurface(layer5Surface);

	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 6, layer6Surface->w, layer6Surface->h, 1, GL_RGBA, GL_UNSIGNED_BYTE, layer6Surface->pixels);
	SDL_DestroySurface(layer6Surface);

	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 7, layer7_lightsSurface->w, layer7_lightsSurface->h, 1, GL_RGBA, GL_UNSIGNED_BYTE, layer7_lightsSurface->pixels);
	SDL_DestroySurface(layer7_lightsSurface);

	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 8, layer8Surface->w, layer8Surface->h, 1, GL_RGBA, GL_UNSIGNED_BYTE, layer8Surface->pixels);
	SDL_DestroySurface(layer8Surface);

	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 9, layer9Surface->w, layer9Surface->h, 1, GL_RGBA, GL_UNSIGNED_BYTE, layer9Surface->pixels);
	SDL_DestroySurface(layer9Surface);

	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 10, layer10Surface->w, layer10Surface->h, 1, GL_RGBA, GL_UNSIGNED_BYTE, layer10Surface->pixels);
	SDL_DestroySurface(layer10Surface);

	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 11, layer11Surface->w, layer11Surface->h, 1, GL_RGBA, GL_UNSIGNED_BYTE, layer11Surface->pixels);
	SDL_DestroySurface(layer11Surface);

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	glActiveTexture(GL_TEXTURE31);

	// MOVE TO INIT!!!


	glGenFramebuffers(1, &FBO);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &screenTexture);
	glBindTexture(GL_TEXTURE_2D, screenTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);

	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1920, 1080);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	float rectVertices12[] = {
		1920.0f,    0.0f, 1.0f, 0.0f,
		0.0f,       0.0f, 0.0f, 0.0f,
		0.0f,    1080.0f, 0.0f, 1.0f,

		1920.0f, 1080.0f, 1.0f, 1.0f,
		1920.0f,    0.0f, 1.0f, 0.0f,
		0.0f,    1080.0f, 0.0f, 1.0f
	
	};

	float rectVertices[] = {
	1.0f,    -1.0f, 1.0f, 0.0f,
	-1.0f,       -1.0f, 0.0f, 0.0f,
	-1.0f,    1.0f, 0.0f, 1.0f,

	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f,    -1.0f, 1.0f, 0.0f,
	-1.0f,    1.0f, 0.0f, 1.0f

	};

	glGenVertexArrays(1, &framebufferVAO);
	glGenBuffers(1, &framebufferVBO);
	glBindVertexArray(framebufferVAO);
	glBindBuffer(GL_ARRAY_BUFFER, framebufferVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectVertices), rectVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	framebufferShader->UseInPipeline(pipelineProgramID);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	SetDefaultLayerValues(layer0);
	layer0.textureID = 0;

	SetDefaultLayerValues(layer1);
	layer1.textureID = 1;

	SetDefaultLayerValues(layer2);
	layer2.textureID = 2;

	SetDefaultLayerValues(layer3);
	layer3.textureID = 3;

	SetDefaultLayerValues(layer4_lights);
	layer4_lights.textureID = 4;

	SetDefaultLayerValues(layer5);
	layer5.textureID = 5;

	SetDefaultLayerValues(layer6);
	layer6.textureID = 6;
	 
	SetDefaultLayerValues(layer7_lights);
	layer7_lights.textureID = 7;

	SetDefaultLayerValues(layer8);
	layer8.textureID = 8;

	SetDefaultLayerValues(layer9);
	layer9.textureID = 9;

	SetDefaultLayerValues(layer10);
	layer10.speed_modifier = 1.0f;
	layer10.textureID = 10;

	SetDefaultLayerValues(layer11);
	layer11.speed_modifier = 1.0f;
	layer11.textureID = 11;

	layer0.speed_modifier = 0.35f;
	layer1.speed_modifier = 0.40f;
	layer2.speed_modifier = 0.45f;
	layer3.speed_modifier = 0.5f;
	layer4_lights.speed_modifier = 0.55f;
	layer5.speed_modifier = 0.6f;
	layer6.speed_modifier = 0.65f;
	layer7_lights.speed_modifier = 0.7f;
	layer8.speed_modifier = 0.75f;
	layer9.speed_modifier = 0.8f;



}

void BackgroundRenderer::RenderLevelBackground(ShaderProgram* shader, ShaderProgram* framebufferShader, GLuint& pipelineProgramID, const glm::mat4& projection, const float& blackHoleAABBx, glm::vec2 blackHoleCenterPos, const float& camPosx, bool gameStarted, const float& delta) {
	shader->UseInPipeline(pipelineProgramID);

	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE30);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);


	shader->SetMat4("uModelMatrix", glm::mat4(1.0f));
	shader->SetMat4("uProjectionMatrix", projection);
	shader->SetInt("uLevelBackground", 1);
	shader->SetInt("uTextureArray", 30);

	std::vector<Box> all_vertecies;

	{
		std::vector<Box> v = GetBoxes(layer0);
		all_vertecies.insert(all_vertecies.end(), v.begin(), v.end());
	} {
		std::vector<Box> v = GetBoxes(layer1);
		all_vertecies.insert(all_vertecies.end(), v.begin(), v.end());
	} {
		std::vector<Box> v = GetBoxes(layer2);
		all_vertecies.insert(all_vertecies.end(), v.begin(), v.end());
	} {
		std::vector<Box> v = GetBoxes(layer3);
		all_vertecies.insert(all_vertecies.end(), v.begin(), v.end());
	} {
		std::vector<Box> v = GetBoxes(layer4_lights);
		all_vertecies.insert(all_vertecies.end(), v.begin(), v.end());
	} {
		std::vector<Box> v = GetBoxes(layer5);
		all_vertecies.insert(all_vertecies.end(), v.begin(), v.end());
	} {
		std::vector<Box> v = GetBoxes(layer6);
		all_vertecies.insert(all_vertecies.end(), v.begin(), v.end());
	} {
		std::vector<Box> v = GetBoxes(layer7_lights);
		all_vertecies.insert(all_vertecies.end(), v.begin(), v.end());
	} {
		std::vector<Box> v = GetBoxes(layer8);
		all_vertecies.insert(all_vertecies.end(), v.begin(), v.end());
	} {
		std::vector<Box> v = GetBoxes(layer9);
		all_vertecies.insert(all_vertecies.end(), v.begin(), v.end());
	} {
		std::vector<Box> v = GetBoxes(layer10);
		all_vertecies.insert(all_vertecies.end(), v.begin(), v.end());
	} {
		std::vector<Box> v = GetBoxes(layer11);
		all_vertecies.insert(all_vertecies.end(), v.begin(), v.end());
	}

	std::reverse(all_vertecies.begin(), all_vertecies.end());

	glBufferSubData(GL_ARRAY_BUFFER, 0, all_vertecies.size() * sizeof(Box), all_vertecies.data());


	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glClearColor((14.0f / 256.0f), (7.0f / 256.0f), (27.0f / 256.0f), 0.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glDrawElements(GL_TRIANGLES, 216, GL_UNSIGNED_INT, nullptr);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	framebufferShader->UseInPipeline(pipelineProgramID);
	framebufferShader->SetInt("uScreenTexture", 31);
	framebufferShader->SetVec2("uBlackHoleCenterPos", glm::vec2((blackHoleCenterPos.x - camPosx) / 1920.0f * currentResolution.x, blackHoleCenterPos.y / 1080.0f * currentResolution.y));
	framebufferShader->SetVec2("uCurrentResolution", currentResolution);

	if (gameStarted) {
		framebufferShader->SetFloat("uBlackHoleAABBx", (blackHoleAABBx - camPosx + 150) / 1920.0f * currentResolution.x);
	}
	else {
		framebufferShader->SetFloat("uBlackHoleAABBx", -300);
	}


	framebufferShader->SetFloat("delta", delt);
	glBindVertexArray(framebufferVAO);
	glActiveTexture(GL_TEXTURE31);
	glBindTexture(GL_TEXTURE_2D, screenTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	time += delta;

	if (time > 0.075f) {
		time = 0;
		
		delt = delta;
	}


}

void BackgroundRenderer::Update(const glm::vec2& camPos, const glm::vec2& actorVelocity, const float& deltaTime) {
	LayerPositionUpdate(layer0, camPos, actorVelocity, deltaTime);
	LayerPositionUpdate(layer1, camPos, actorVelocity, deltaTime);
	LayerPositionUpdate(layer2, camPos, actorVelocity, deltaTime);
	LayerPositionUpdate(layer3, camPos, actorVelocity, deltaTime);
	LayerPositionUpdate(layer4_lights, camPos, actorVelocity, deltaTime);
	LayerPositionUpdate(layer5, camPos, actorVelocity, deltaTime);
	LayerPositionUpdate(layer6, camPos, actorVelocity, deltaTime);
	LayerPositionUpdate(layer7_lights, camPos, actorVelocity, deltaTime);
	LayerPositionUpdate(layer8, camPos, actorVelocity, deltaTime);
	LayerPositionUpdate(layer9, camPos, actorVelocity, deltaTime);
	LayerPositionUpdate(layer10, camPos, actorVelocity, deltaTime);
	LayerPositionUpdate(layer11, camPos, actorVelocity, deltaTime);
}

void BackgroundRenderer::SetDefaultLayerValues(LevelBackgroundLayer& layer) {
	layer.b1.Size = backgroundSize;
	layer.b2.Size = backgroundSize;
	layer.b3.Size = backgroundSize;

	layer.b1.Position = glm::vec2(0.0f, 0.0f);
	layer.b2.Position = glm::vec2(backgroundSize.x, 0.0f);
	layer.b3.Position = glm::vec2(backgroundSize.x * 2, 0.0f);

	layer.offset = 0;
}

void BackgroundRenderer::LayerPositionUpdate(LevelBackgroundLayer& layer, const glm::vec2& camPos, const glm::vec2& actorVelocity, const float& deltaTime) {
	if (layer.b1.Position.x + backgroundSize.x < camPos.x && camPos.x + 960.0f > layer.b3.Position.x) {
		layer.offset++;

		layer.b1.Position.x += backgroundSize.x;
		layer.b2.Position.x += backgroundSize.x;
		layer.b3.Position.x += backgroundSize.x;
	}
	else if (layer.b3.Position.x > camPos.x + 1920.0f && camPos.x + 960.0f < layer.b1.Position.x + layer.b1.Size.x) {
		layer.offset--;

		layer.b1.Position.x -= backgroundSize.x;
		layer.b2.Position.x -= backgroundSize.x;
		layer.b3.Position.x -= backgroundSize.x;
	}

	layer.b1.Position.x += (actorVelocity.x * layer.speed_modifier * deltaTime);
	layer.b2.Position.x += (actorVelocity.x * layer.speed_modifier * deltaTime);
	layer.b3.Position.x += (actorVelocity.x * layer.speed_modifier * deltaTime);
}

std::vector<Box> BackgroundRenderer::GetBoxes(LevelBackgroundLayer& layer) {

	std::vector<Box> boxes;

	{
		Box b;
		b.Position = glm::vec2(layer.b1.Position.x - 1.0f, layer.b1.Position.y);
		b.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		b.TexturePosition = glm::vec2(0.0f, 0.0f);
		b.TextureIndex = layer.textureID;
		boxes.push_back(b);
	} {
		Box b;
		b.Position = glm::vec2(layer.b1.Position.x + layer.b1.Size.x + 1.0f, layer.b1.Position.y);
		b.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		b.TexturePosition = glm::vec2(1.0f, 0.0f);
		b.TextureIndex = layer.textureID;
		boxes.push_back(b);
	} {
		Box b;
		b.Position = glm::vec2(layer.b1.Position.x + layer.b1.Size.x + 1.0f, layer.b1.Position.y + layer.b1.Size.y);
		b.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		b.TexturePosition = glm::vec2(1.0f, 1.0f);
		b.TextureIndex = layer.textureID;
		boxes.push_back(b);
	} {
		Box b;
		b.Position = glm::vec2(layer.b1.Position.x - 1.0f, layer.b1.Position.y + layer.b1.Size.y);
		b.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		b.TexturePosition = glm::vec2(0.0f, 1.0f);
		b.TextureIndex = layer.textureID;
		boxes.push_back(b);
	}
	{
		Box b;
		b.Position = glm::vec2(layer.b2.Position.x - 1.0f, layer.b2.Position.y);
		b.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		b.TexturePosition = glm::vec2(0.0f, 0.0f);
		b.TextureIndex = layer.textureID;
		boxes.push_back(b);
	} {
		Box b;
		b.Position = glm::vec2(layer.b2.Position.x + layer.b2.Size.x + 1.0f, layer.b2.Position.y);
		b.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		b.TexturePosition = glm::vec2(1.0f, 0.0f);
		b.TextureIndex = layer.textureID;
		boxes.push_back(b);
	} {
		Box b;
		b.Position = glm::vec2(layer.b2.Position.x + layer.b2.Size.x + 1.0f, layer.b2.Position.y + layer.b2.Size.y);
		b.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		b.TexturePosition = glm::vec2(1.0f, 1.0f);
		b.TextureIndex = layer.textureID;
		boxes.push_back(b);
	} {
		Box b;
		b.Position = glm::vec2(layer.b2.Position.x - 1.0f, layer.b2.Position.y + layer.b2.Size.y);
		b.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		b.TexturePosition = glm::vec2(0.0f, 1.0f);
		b.TextureIndex = layer.textureID;
		boxes.push_back(b);
	}
	{
		Box b;
		b.Position = glm::vec2(layer.b3.Position.x - 1.0f, layer.b3.Position.y);
		b.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		b.TexturePosition = glm::vec2(0.0f, 0.0f);
		b.TextureIndex = layer.textureID;
		boxes.push_back(b);
	} {
		Box b;
		b.Position = glm::vec2(layer.b3.Position.x + layer.b3.Size.x + 1.0f, layer.b3.Position.y);
		b.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		b.TexturePosition = glm::vec2(1.0f, 0.0f);
		b.TextureIndex = layer.textureID;
		boxes.push_back(b);
	} {
		Box b;
		b.Position = glm::vec2(layer.b3.Position.x + layer.b3.Size.x + 1.0f, layer.b3.Position.y + layer.b3.Size.y);
		b.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		b.TexturePosition = glm::vec2(1.0f, 1.0f);
		b.TextureIndex = layer.textureID;
		boxes.push_back(b);
	} {
		Box b;
		b.Position = glm::vec2(layer.b3.Position.x - 1.0f, layer.b3.Position.y + layer.b3.Size.y);
		b.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		b.TexturePosition = glm::vec2(0.0f, 1.0f);
		b.TextureIndex = layer.textureID;
		boxes.push_back(b);
	}
	return boxes;
}

void BackgroundRenderer::ResizeFramebuffer(glm::vec2 resolution) {
	glActiveTexture(GL_TEXTURE31);

	glBindTexture(GL_TEXTURE_2D, screenTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, resolution.x, resolution.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, resolution.x, resolution.y);

	currentResolution = resolution;
}