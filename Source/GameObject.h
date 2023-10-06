#pragma once
#include <glm/glm.hpp>

#include "SpriteRenderer.h"
#include "../Headers/Texture.h"

class GameObject
{
public:
	glm::vec2 pre_position;
	glm::vec2 position, size, velocity;
	glm::vec3 color;
	float rotation;
	bool is_solid;
	bool is_destory;
	Texture texture;

public:
	GameObject();
	GameObject(Texture& texture, glm::vec2 position, glm::vec2 size, glm::vec2 velocity = glm::vec2(0.0f), glm::vec3 color = glm::vec3(1.0f));
	virtual	void Draw(SpriteRenderer& renderer);
};
