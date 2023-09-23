#include "BallObject.h"

BallObject::BallObject() :GameObject(), is_stack(true), radius(12.5f)
{
}

BallObject::BallObject(Texture& texture, glm::vec2 position, glm::vec2 size, float radius, glm::vec2 velocity) :GameObject(texture, position, size, velocity), radius(radius), is_stack(true)
{
}

glm::vec2 BallObject::Move(float dt, unsigned int width)
{
	if (!is_stack)
	{
		this->position += this->velocity * dt;
		if (this->position.x < 0.0f)
		{
			this->velocity.x = -this->velocity.x;
			this->position.x = 0.0f;
		}
		else if (this->position.x + this->size.x > width)
		{
			this->velocity.x = -this->velocity.x;
			this->position.x = width - this->size.x;
		}

		if (this->position.y < 0.0f)
		{
			this->velocity.y = -this->velocity.y;
			this->position.y = 0.0f;
		}
	}

	return this->position;
}

void BallObject::Reset(glm::vec2 position, glm::vec2 velocity)
{
	this->position = position;
	this->velocity = velocity;
}