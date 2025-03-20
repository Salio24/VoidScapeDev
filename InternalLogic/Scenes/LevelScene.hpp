#pragma once 
#include <string>
#include <nlohmannjson/json.hpp>
#include "../../Actors/GameObject.hpp"
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>

class LevelScene {
public:
	LevelScene();
	
	~LevelScene();

	void LoadLevel(std::string path, const int& tilesetOffset);

	void LoadLevelTMX(std::string path, const int& tilesetOffset, glm::vec2& spawn);

	std::vector<GameObject> mLevelBlocks;

	std::string toLower(const std::string& str) {
		std::string result = str;
		std::transform(result.begin(), result.end(), result.begin(),
			[](unsigned char c) { return std::tolower(c); });
		return result;
	}

private:
	int blockSize{ 18 };

};