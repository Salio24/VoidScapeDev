#pragma once 
#include <string>
#include <nlohmannjson/json.hpp>
#include "GameObject.hpp"
#include <fstream>
#include <vector>

class LevelScene {
public:
	LevelScene();
	
	~LevelScene();

	void LoadLevel(std::string path, const int& tilesetOffset);

	std::vector<GameObject> mLevelBlocks;

private:
	int blockSize{ 18 };

};