#include "LevelScene.hpp"
#include <fstream>
#include <tmxlite/Map.hpp>
#include <tmxlite/Layer.hpp>
#include <tmxlite/TileLayer.hpp>
#include <tmxlite/ObjectGroup.hpp>
#include <iostream>


LevelScene::LevelScene() {

}

LevelScene::~LevelScene() {

}

void LevelScene::LoadLevel(std::string path, const int& tilesetOffset) {

	// DEPRECATED!

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

void LevelScene::LoadLevelTMX(std::string path, const int& tilesetOffset, glm::vec2& spawn) {
	
	tmx::Object::Shape shape;

	tmx::Map map;
	
	static int GDI = 0;

	bool spFound = false;

	if (map.load(path)) {
		const auto& layers = map.getLayers();

		const auto gridSize = map.getTileSize();

		const auto mapSize = map.getBounds();

		for (const auto& tileset : map.getTilesets()) {
			if (toLower(tileset.getName()) == "tiles32") {
				GDI = tileset.getFirstGID();
			}
		}

		for (const auto& layer : layers) {
			if (layer->getType() == tmx::Layer::Type::Object) {
				const auto& objectLayer = layer->getLayerAs<tmx::ObjectGroup>();

				if (toLower(objectLayer.getName()) == "colliders") {
					const auto& objects = objectLayer.getObjects();
					for (const auto& object : objects) {
						auto aabb = object.getAABB();

						GameObject obj;
						obj.mIsCollidable = true;
						obj.mSprite.mVertexData.Position = glm::vec2(((aabb.left) / gridSize.x) * blockSize, ((mapSize.height - (aabb.top + aabb.height)) / gridSize.y) * blockSize);
						obj.mSprite.mVertexData.Size = glm::vec2((aabb.width / gridSize.x) * blockSize, (aabb.height / gridSize.y) * blockSize);
						obj.mIsVisible = false;

						obj.mSprite.mVertexData.TextureIndex = 0;


						this->mLevelBlocks.push_back(obj);
					}
				}
				else if (toLower(objectLayer.getName()) == "triggers") {
					const auto& objects = objectLayer.getObjects();

					for (const auto& object : objects) {
						if (toLower(object.getName()) == "actor spawn" && object.getShape() == tmx::Object::Shape::Point) {
							const auto pos = object.getPosition();

							spawn = glm::vec2((pos.x / gridSize.x) * blockSize, ((mapSize.height - pos.y) / gridSize.y) * blockSize);

							spFound = true;
						}
					}

					
				}

			}
			else if (layer->getType() == tmx::Layer::Type::Tile) {
				const auto& tileLayer = layer->getLayerAs<tmx::TileLayer>();

				if (toLower(tileLayer.getName()) == "baselayer") {
					const auto& tiles = tileLayer.getTiles();

					int height = layer->getSize().y;
					int width = layer->getSize().x;
					
					for (int i = 0; i < height; ++i) {
						for (int j = 0; j < width; ++j) {
							if (tiles[width * i + j].ID != 0) {
								GameObject obj;
								obj.mSprite.mVertexData.Position = glm::vec2(j * blockSize, ((height - i) * blockSize) - blockSize);
								obj.mSprite.mVertexData.Size = glm::vec2(blockSize, blockSize);
								obj.mIsVisible = true;
								obj.mSprite.mVertexData.TextureIndex = tilesetOffset + tiles[width * i + j].ID - GDI;

								this->mLevelBlocks.push_back(obj);
							}
						}
					}
				}
			}
		}

		if (!spFound) {
			std::cout << "Spawn point not specified on the level, defaulting" << std::endl;
			spawn = glm::vec2(500.0f);
		}

		const auto& tilesets = map.getTilesets();
		for (const auto& tileset : tilesets) {
			//read out tile set properties, load textures etc...
		}
	}

}