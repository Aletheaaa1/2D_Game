#include "ParticleGenerator.h"

ParticleGenerator::ParticleGenerator(Texture& texture, Shader& shader, unsigned int amount) :shader(shader), texture(texture), amount(amount)
{
	this->Init();
}

void ParticleGenerator::Init()
{
	for (unsigned int i = 0; i < this->amount; i++)
	{
		this->particles.push_back(Particle());
	}

	float vertices[] = {
		// pos      // tex
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	this->vao.Bind();
	this->vao.AddBuffer(0, 2, GL_FLOAT, false, 4 * sizeof(float), 0);
	this->vao.AddBuffer(1, 2, GL_FLOAT, false, 4 * sizeof(float), 2 * sizeof(float));
}

void ParticleGenerator::Update(float dt, GameObject& obj, unsigned int new_particles, glm::vec2 offset)
{
	//	添加新粒子
	for (unsigned int i = 0; i < new_particles; i++)
	{
		int dead_particle_index = this->FirstUnusedParticle();
		this->RespawnParticle(this->particles[dead_particle_index], obj, offset);
	}

	//	更新所有粒子
	for (unsigned int i = 0; i < this->amount; i++)
	{
		Particle& p = this->particles[i];
		p.life -= dt;
		if (p.life > 0.0f)
		{
			p.position -= p.velocity * dt;
			p.color.a -= dt * 2.5f;
		}
	}
}

void ParticleGenerator::Draw()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	this->texture.Usetexture(0);
	this->texture.Bind();
	this->shader.Bind();
	for (auto p : particles)
	{
		if (p.life > 0.0f)
		{
			this->shader.SetUniform2v("offset", p.position);
			this->shader.SetUniform4f("color", glm::value_ptr(p.color));
			this->vao.Bind();
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

unsigned int last_dead_particle = 0;
unsigned int ParticleGenerator::FirstUnusedParticle()
{
	//	从消亡粒子开始找
	for (unsigned int i = last_dead_particle; i < this->amount; i++)
	{
		if (this->particles[i].life <= 0.0f)
		{
			last_dead_particle = i;
			return i;
		}
	}

	//	若不存在则从头开始
	for (unsigned int i = 0; i < last_dead_particle; i++)
	{
		if (this->particles[i].life <= 0.0f)
		{
			last_dead_particle = i;
			return i;
		}
	}

	//若所有粒子都存在则返回第一个粒子
	last_dead_particle = 0;
	return 0;
}

void ParticleGenerator::RespawnParticle(Particle& particle, GameObject& obj, glm::vec2 offset)
{
	float random_position = ((rand() % 100) - 50) / 10.0f;
	float r_color = ((rand() % 100) / 100.0f);
	particle.position = obj.position + random_position + offset;
	particle.life = 1.0f;
	particle.color = glm::vec4(r_color, r_color, r_color, 1.0f);
	particle.velocity = obj.velocity * 0.1f;
}