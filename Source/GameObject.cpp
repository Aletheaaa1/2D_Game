#include "GameObject.h"

GameObject::GameObject() : position(0, 0), size(1, 1), velocity(0.0f), color(1.0f), rotation(0.0f), texture(), is_solid(true), is_destory(false)
{
}

GameObject::GameObject(Texture& texture, glm::vec2 position, glm::vec2 size, glm::vec2 velocity /*= glm::vec2(0.0f)*/, glm::vec3 color /*= glm::vec3(1.0f)*/) :texture(texture), position(position), size(size), velocity(velocity), rotation(0.0f), color(color), is_solid(true), is_destory(false)
{
}

void GameObject::Draw(SpriteRenderer& renderer)
{
	renderer.DrawSprite(this->texture, this->position, this->size, this->rotation, this->color);
}