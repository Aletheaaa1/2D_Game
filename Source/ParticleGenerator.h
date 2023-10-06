#pragma once
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <vector>

#include "BallObject.h"
#include "../Headers/Shader.h"
#include "../Headers/Texture.h"
#include "../Headers/ResourceManager.h"
#include "../Headers/VertexArrayObject.h"

struct Particle
{
	float life;
	glm::vec4 color;
	glm::vec2 position, velocity;

	Particle() :position(0.0f), velocity(0.0f), color(1.0f), life(0.0f) {}
};

class ParticleGenerator
{
private:
	std::vector<Particle> particles;
	unsigned int amount;
	Shader shader;
	Texture texture;
	VertexArrayObject vao;

public:
	ParticleGenerator(Texture& texture, Shader& shader, unsigned int amount);
	void Draw();
	void Update(float dt, BallObject& obj, unsigned int new_particles, glm::vec2 offset = glm::vec2{ 0.0f });

private:
	//	��ʼ��
	void Init();

	//	�ҵ���һ����������
	unsigned int FirstUnusedParticle();

	//	��������
	void RespawnParticle(Particle& particle, BallObject& obj, glm::vec2 offset);
};
