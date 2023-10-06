#pragma once
#include "GameObject.h"

const glm::vec2 VELOCITY{ 0.0f, 150.0f };
const glm::vec2 POWERUP_SIZE{ 60.0f, 20.0f };

class PowerUp : public GameObject
{
public:
	std::string type;
	float duration_time;
	bool is_actived;
	PowerUp(Texture& texture, std::string type, float duration_time, glm::vec2 position, glm::vec3 color) : GameObject{ texture, position, POWERUP_SIZE, VELOCITY }, type(type), duration_time(duration_time), is_actived(false) {}
};