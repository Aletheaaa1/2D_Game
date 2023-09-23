#pragma once
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#include "SpriteRenderer.h"
#include "GameObject.h"
#include "../Headers/ResourceManager.h"

class GameLevel
{
public:
	std::vector<GameObject> bricks;
public:
	GameLevel();
	void Load(const char* file, unsigned int level_width, unsigned int level_height);
	void Draw(SpriteRenderer& renderer);
	bool IsCompleted();
private:
	void Init(std::vector<std::vector<unsigned int>> brick_data, unsigned int level_width, unsigned int level_height);
};
