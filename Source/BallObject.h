#pragma once
#include <glm/glm.hpp>
#include <vector>

#include "../Headers/Texture.h"
#include "GameObject.h"

class BallObject :public GameObject
{
public:
	bool is_stick;
	float radius;

public:
	BallObject();
	BallObject(Texture& texture, glm::vec2 position, glm::vec2 size, float radius, glm::vec2 velocity);
	glm::vec2 Move(float dt, unsigned int width);
	void Reset(glm::vec2 position, glm::vec2 velocity);
};
