#include "LevelScene.hpp"
#include <fstream>

LevelScene::LevelScene() {

}

LevelScene::~LevelScene() {

}

void LevelScene::LoadLevel(std::string path, const int& tilesetOffset) {

	std::ifstream jsonStream(path);

	nlohmann::json GameLevelDataJson = nlohmann::json::parse(jsonStream);

	int levelWidth = GameLevelDataJson["width"];
	int levelHeight = GameLevelDataJson["height"];

	std::vector<int> tilesetsOffsets;

	std::vector<std::vector<std::vector<int>>> levelData;
	
	for (int i = 0; i < GameLevelDataJson["layers"].size(); i++) {
		if (GameLevelDataJson["layers"][i]["name"] == "BaseLayer") {
			std::vector<std::vector<int>> data(levelHeight, std::vector<int>(levelWidth));
			std::vector vec = GameLevelDataJson["layers"][i]["data"].get<std::vector<int>>();
			for (int j = 0; j < levelHeight; j++) {
				for (int k = 0; k < levelWidth; k++) {
					data[j][k] = vec[j * levelWidth + k];
				}
			}
			tilesetsOffsets.push_back(GameLevelDataJson["tilesets"][i]["firstgid"]);
			levelData.push_back(data);
		}
		else if (GameLevelDataJson["layers"][i]["name"] == "CollisionLayer") {
			std::vector<std::vector<int>> data(levelHeight, std::vector<int>(levelWidth));
			std::vector vec = GameLevelDataJson["layers"][i]["data"].get<std::vector<int>>();
			for (int j = 0; j < levelHeight; j++) {
				for (int k = 0; k < levelWidth; k++) {
					data[j][k] = vec[j * levelWidth + k];
				}
			}
			tilesetsOffsets.push_back(GameLevelDataJson["tilesets"][i]["firstgid"]);
			levelData.push_back(data);
		}
	}
	for (int i = 0; i < levelHeight; ++i) {
		for (int j = 0; j < levelWidth; ++j) {
			if (levelData[0][i][j] != 0 || levelData[1][i][j] != 0) {
				GameObject obj;
				obj.mSprite.mVertexData.Position = glm::vec2(j * blockSize, ((levelHeight - i) * blockSize) - blockSize);
				obj.mSprite.mVertexData.Size = glm::vec2(blockSize, blockSize);
				if (levelData[0][i][j] != 0) {
					obj.mSprite.mVertexData.TextureIndex = tilesetOffset + levelData[0][i][j] - 1;
					obj.mIsVisible = true;
				}
				else {
					obj.mIsVisible = false;
				}
				if ((levelData[1][i][j] - tilesetsOffsets[1]) + 1 == 1 || (levelData[1][i][j] - tilesetsOffsets[1]) + 1 == 15) {
					obj.mIsCollidable = true;
				}
				else if ((levelData[1][i][j] - tilesetsOffsets[1]) + 1 == 2) {
					obj.mIsCollidable = false;
					obj.mTriggerAABBPos = glm::vec2(obj.mSprite.mVertexData.Position.x + (float)blockSize * (3.0f / 8.0f), obj.mSprite.mVertexData.Position.y);
					obj.mTriggerAABBSize = glm::vec2(blockSize / 4, blockSize);
					obj.mIsDeathTrigger = true;
				}
				else if ((levelData[1][i][j] - tilesetsOffsets[1]) + 1 == 3) {
					obj.mIsCollidable = false;
					obj.mTriggerAABBPos = glm::vec2(obj.mSprite.mVertexData.Position.x + (float)blockSize * (3.0f / 8.0f), obj.mSprite.mVertexData.Position.y);
					obj.mTriggerAABBSize = glm::vec2(blockSize / 4, blockSize / 2);
					obj.mIsDeathTrigger = true;
				}
				else if ((levelData[1][i][j] - tilesetsOffsets[1]) + 1 == 4) {
					obj.mIsCollidable = true;
					obj.mTriggerAABBPos = obj.mSprite.mVertexData.Position;
					obj.mTriggerAABBSize = obj.mSprite.mVertexData.Size;
					obj.mIsDeathTrigger = true;
				}
				else {
					obj.mIsCollidable = false;
				}
				this->mLevelBlocks.push_back(obj);
			}
		}
	}
}
