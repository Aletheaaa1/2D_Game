#include "SpriteRenderer.h"

SpriteRenderer::SpriteRenderer(Shader& shader) :shader(shader)
{
	this->Init();
}

SpriteRenderer::~SpriteRenderer()
{
}

void SpriteRenderer::DrawSprite(Texture& texture, glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color)
{
	this->shader.Bind();
	texture.Usetexture(0);
	texture.Bind();

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(position, 0.0f));
	model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
	model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));
	model = glm::scale(model, glm::vec3(size, 1.0f));

	this->shader.SetUniformMat4("model", model);
	this->shader.SetUniform3v("spriteColor", color);

	this->vao.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void SpriteRenderer::Init()
{
	float vertices[] = {
		// pos      // tex
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};

	unsigned int VBO;
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	this->vao.Bind();
	this->vao.AddBuffer(0, 2, GL_FLOAT, false, 4 * sizeof(float), 0);
	this->vao.AddBuffer(1, 2, GL_FLOAT, false, 4 * sizeof(float), 2 * sizeof(float));
}