#include "GameLevel.h"

GameLevel::GameLevel()
{
}

void GameLevel::Load(const char* file, unsigned int level_width, unsigned int level_height)
{
	this->bricks.clear();
	std::ifstream fstream(file);
	std::string line;
	unsigned int  brick_level;
	std::vector<std::vector<unsigned int>> brick_data;
	if (fstream)
	{
		while (std::getline(fstream, line, '\n'))
		{
			std::istringstream ss(line);
			std::vector<unsigned int> row;
			while (ss >> brick_level)
			{
				row.push_back(brick_level);
			}
			brick_data.push_back(row);
		}
	}
	if (brick_data.size() > 0)
	{
		this->Init(brick_data, level_width, level_height);
	}
}

void GameLevel::Draw(SpriteRenderer& renderer)
{
	for (auto& brick : this->bricks)
	{
		if (!brick.is_destory)
		{
			brick.Draw(renderer);
		}
	}
}

bool GameLevel::IsCompleted()
{
	for (auto& brick : this->bricks)
	{
		if (!brick.is_destory && !brick.is_solid)
		{
			return false;
		}
	}
	return true;
}

void GameLevel::Init(std::vector<std::vector<unsigned int>> bricks_data, unsigned int level_width, unsigned int level_height)
{
	unsigned int height = bricks_data.size();
	unsigned int width = bricks_data.at(0).size();
	float unit_width = static_cast<float>(level_width) / static_cast<float>(width);
	float unit_height = static_cast<float>(level_height) / static_cast<float>(height);

	for (unsigned int y = 0; y < height; y++)
	{
		for (unsigned int x = 0; x < width; x++)
		{
			if (bricks_data[y][x] == 1)
			{
				glm::vec2 position = glm::vec2{ unit_width * x, unit_height * y };
				glm::vec2 size = glm::vec2{ unit_width, unit_height };
				glm::vec3 color = glm::vec3{ 0.8f, 0.8f, 0.7f };

				GameObject obj = GameObject{ ResourceManager::GetTexture("block_solid"), position, size ,glm::vec2{0.0f, 0.0f}, color };
				obj.is_solid = true;
				this->bricks.push_back(obj);
			}
			else if (bricks_data[y][x] > 1)
			{
				glm::vec3 color = glm::vec3{ 1.0f };
				if (bricks_data[y][x] == 2)
					color = glm::vec3{ 0.2f, 0.6f, 1.0f };
				else if (bricks_data[y][x] == 3)
					color = glm::vec3{ 0.0f, 0.7f, 0.0f };
				else if (bricks_data[y][x] == 4)
					color = glm::vec3{ 0.8f, 0.8f, 0.4f };
				else if (bricks_data[y][x] == 5)
					color = glm::vec3{ 1.0f, 0.5f, 0.0f };

				glm::vec2 position = glm::vec2{ unit_width * x, unit_height * y };
				glm::vec2 size = glm::vec2{ unit_width, unit_height };
				GameObject obj{ ResourceManager::GetTexture("block"), position, size, glm::vec2{0.0f, 0.0f}, color };
				obj.is_solid = false;
				this->bricks.push_back(obj);
			}
		}
	}
}