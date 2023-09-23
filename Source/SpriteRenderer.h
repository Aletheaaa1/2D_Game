#pragma once
#include <glm/glm.hpp>

#include "../Headers/Shader.h"
#include "../Headers/Texture.h"
#include "../Headers/VertexArrayObject.h"
#include "../Headers/VertexBufferObject.h"

class SpriteRenderer
{
private:
	Shader shader;
	VertexArrayObject vao;
public:
	SpriteRenderer(Shader& shader);
	~SpriteRenderer();
	void DrawSprite(Texture& texture, glm::vec2 position, glm::vec2 size, float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));

private:
	void Init();
};
