#include "Game.h"

Game::Game(unsigned int screen_width, unsigned int screen_height) :state(GameState::GAME_ACTIVE), screen_width(screen_width), screen_height(screen_height), keys()
{
}

void Game::Init()
{
	Shader shader = ResourceManager::LoadShader("./Shaders/sprite.vs", "./Shaders/sprite.fs", "", "sprite_shader");
	Shader particle_shader = ResourceManager::LoadShader("./Shaders/particle.vs", "./Shaders/particle.fs", "", "particle_shader");
	Shader post_processor_shader = ResourceManager::LoadShader("./Shaders/post_processor.vs", "./Shaders/post_processor.fs", "", "post_processor_shader");

	Texture particle_texture = ResourceManager::LoadTexture("./Textures/particle.png", false, true, "particle_texture");
	Texture background_texture = ResourceManager::LoadTexture("./Textures/background.jpg", true, false, "background");
	Texture block_texture = ResourceManager::LoadTexture("./Textures/block.png", true, false, "block");
	Texture block_solid_texture = ResourceManager::LoadTexture("./Textures/block_solid.png", true, false, "block_solid");
	Texture player_texture = ResourceManager::LoadTexture("./Textures/paddle.png", true, true, "player");
	Texture ball_texture = ResourceManager::LoadTexture("./Textures/ball.png", true, false, "ball");

	ResourceManager::LoadTexture("./Textures/powerup_speed.png", true, false, "powerup_speed_texture");
	ResourceManager::LoadTexture("./Textures/powerup_sticky.png", true, false, "powerup_sticky_texture");
	ResourceManager::LoadTexture("./Textures/powerup_passthrough.png", true, false, "powerup_passthrough_texture");
	ResourceManager::LoadTexture("./Textures/powerup_increase.png", true, false, "powerup_increase_texture");
	ResourceManager::LoadTexture("./Textures/powerup_confuse.png", true, false, "powerup_confuse_texture");
	ResourceManager::LoadTexture("./Textures/powerup_chaos.png", true, false, "powerup_chaos_texture");

	glm::mat4 projection = glm::ortho(0.0f, float(this->screen_width), float(this->screen_height), 0.0f, -0.0f, 1.0f);
	shader.Bind();
	shader.SetUniformMat4("projection", projection);
	shader.SetUniform1i("image_texture", 0);

	//
	//	Level
	//
	GameLevel level_1, Level_2, Level_3, Level_4;
	level_1.Load("./Level/LevelOne.txt", this->screen_width, this->screen_height / 2.0);
	Level_2.Load("./Level/LevelTwo.txt", this->screen_width, this->screen_height / 2.0);
	Level_3.Load("./Level/LevelThree.txt", this->screen_width, this->screen_height / 2.0);
	Level_4.Load("./Level/LevelFour.txt", this->screen_width, this->screen_height / 2.0);
	this->levels.push_back(level_1);
	this->levels.push_back(Level_2);
	this->levels.push_back(Level_3);
	this->levels.push_back(Level_4);
	this->level = 2;

	//
	//	Renderer
	//
	this->renderer = std::make_unique<SpriteRenderer>(shader);

	//
	//	Player
	//
	glm::vec2 player_pos = glm::vec2{ (this->screen_width - PLAYER_SIZE.x) / 2.0f, this->screen_height - PLAYER_SIZE.y };
	this->player = std::make_unique <GameObject>(player_texture, player_pos, PLAYER_SIZE);

	//
	//	Ball
	//
	glm::vec2 ball_pos = player_pos + glm::vec2{ PLAYER_SIZE.x / 2.0 - BALL_RADIUS , -BALL_RADIUS * 2 };
	this->ball = std::make_unique<BallObject>(ball_texture, ball_pos, BALL_SIZE, BALL_RADIUS, INITIAL_BALL_VELOCITY);

	//
	//	Particle
	//
	particle_shader.Bind();
	particle_shader.SetUniformMat4("projection", projection);
	particle_shader.SetUniform1i("image_texture", 0);
	this->particle = std::make_unique<ParticleGenerator>(particle_texture, particle_shader, 70);

	//
	//	PostProcessor
	//
	this->post_processor = std::make_unique<PostProcessor>(post_processor_shader, this->screen_width, this->screen_height);
}

void Game::Render()
{
	if (this->state == GameState::GAME_ACTIVE)
	{
		this->post_processor->BeginRender();

		this->renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(this->screen_width, this->screen_height), 0.0f);

		this->levels[this->level].Draw(*renderer);

		this->player->Draw(*renderer);

		if (!this->ball->is_stick)
			this->particle->Draw();

		this->ball->Draw(*renderer);

		//	道具
		for (auto& powerup : this->powerups)
		{
			if (!powerup.is_destory)
			{
				powerup.Draw(*renderer);
			}
		}

		this->post_processor->EndRender();
		this->post_processor->Render(glfwGetTime());
	}
}

void Game::Update(float dt)
{
	ball->Move(dt, this->screen_width);

	this->DoCollision();

	//	震动检测
	if (this->post_processor->shake_time > 0.0f)
	{
		this->post_processor->shake_time -= dt;
		if (this->post_processor->shake_time <= 0.0f)
		{
			this->post_processor->is_shake = false;
		}
	}

	this->particle->Update(dt, *ball, 1, glm::vec2(this->ball->radius / 2.0f));

	//	道具
	UpdatePowerUps(dt);

	//	游戏结束后重置
	if (ball->position.y + ball->radius * 2.0f > this->screen_height + 100.0f)
	{
		this->ResetObject();
		this->ResetLevel();
	}
}

//	Reset
void Game::ResetObject()
{
	ball->position = glm::vec2(this->screen_width / 2.0f - ball->radius, this->screen_height - PLAYER_SIZE.y - 2 * ball->radius);
	ball->velocity = INITIAL_BALL_VELOCITY;
	ball->is_stick = true;
	ball->is_pass_through = false;

	player->position = glm::vec2(screen_width / 2.0f - PLAYER_SIZE.x / 2.0f, screen_height - PLAYER_SIZE.y);
	player->color = glm::vec3(1.0f);
	player->size = PLAYER_SIZE;

	post_processor->is_chaos = false;
	post_processor->is_confuse = false;
}

void Game::ResetLevel()
{
	switch (this->level)
	{
	case 0:
		this->levels[0].Load("./Level/LevelOne.txt", this->screen_width, this->screen_height / 2.0f);
		break;
	case 1:
		this->levels[1].Load("./Level/LevelTwo.txt", this->screen_width, this->screen_height / 2.0f);
		break;
	case 2:
		this->levels[2].Load("./Level/LevelThree.txt", this->screen_width, this->screen_height / 2.0f);
		break;
	case 3:
		this->levels[3].Load("./Level/LevelFour.txt", this->screen_width, this->screen_height / 2.0f);
		break;
	}
}

void Game::ProcessInput(float dt)
{
	if (this->state == GameState::GAME_ACTIVE)
	{
		float player_velocity = dt * PLAYER_VELOCITY;
		float ball_velocity = dt * BALL_VELOCITY;
		if (this->keys[GLFW_KEY_D])
		{
			this->player->pre_position = this->player->position;
			if (this->player->position.x <= (this->screen_width - this->player->size.x))
			{
				this->player->position.x += player_velocity;
				if (this->ball->is_stick)
				{
					this->ball->position.x += player_velocity;
				}
			}
		}
		if (this->keys[GLFW_KEY_A])
		{
			this->player->pre_position = this->player->position;
			if (this->player->position.x >= 0)
			{
				this->player->position.x -= player_velocity;
				if (this->ball->is_stick)
				{
					this->ball->position.x += -player_velocity;
				}
			}
		}

		if (this->keys[GLFW_KEY_SPACE])
		{
			this->ball->is_stick = false;
		}
	}
}

void Game::DoCollision()
{
	//	Brick
	for (GameObject& brick : this->levels[this->level].bricks)
	{
		if (!brick.is_destory)
		{
			auto collision = CheckCollision(*ball, brick);
			if (collision.is_collision)
			{
				brick.is_destory = false;
				if (!brick.is_solid)
				{
					brick.is_destory = true;
					//	生成道具
					this->SpawnPowerUps(brick);
				}
				else
				{
					this->post_processor->shake_time = 0.05f;
					this->post_processor->is_shake = true;
				}

				Direction direction = collision.direction;
				glm::vec2 distance = collision.distance;
				//	碰撞反转小球
				if (!ball->is_pass_through || brick.is_solid)
				{
					if (direction == Direction::LEFT || direction == Direction::RIGHT)
					{
						ball->velocity.x = -ball->velocity.x;
						float invasion = ball->radius - std::abs(distance.x);
						if (direction == Direction::LEFT)
						{
							ball->position.x += invasion;
						}
						else if (direction == Direction::RIGHT)
						{
							ball->position.x -= invasion;
						}
					}
					else if (direction == Direction::UP || direction == Direction::DOWN)
					{
						ball->velocity.y = -ball->velocity.y;
						float invasion = ball->radius - std::abs(distance.y);
						if (direction == Direction::UP)
						{
							ball->position.y -= invasion;
						}
						else if (direction == Direction::DOWN)
						{
							ball->position.y += invasion;
						}
					}
				}
			}
		}
	}

	//	Player
	auto collision_player = CheckCollision(*ball, *player);
	if (!ball->is_stick && collision_player.is_collision)
	{
		float center = player->position.x + player->size.x / 2.0f;
		float distance = std::abs(ball->position.x + ball->radius - center);
		float percentage = distance / (player->size.x / 2.0f);
		float strength = 2.0f;

		glm::vec2 old_velocity = ball->velocity;
		ball->velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
		ball->velocity.y = -1 * std::abs(ball->velocity.y);
		//	平衡球的力量和速度
		ball->velocity = glm::normalize(ball->velocity) * glm::length(old_velocity);
	}

	//	Powerup
	for (auto& powerup : this->powerups)
	{
		if (!powerup.is_destory)
		{
			if (powerup.position.y > this->screen_height)
			{
				powerup.is_destory = true;
			}
			if (CheckCollision(*player, powerup))
			{
				powerup.is_destory = true;
				powerup.is_actived = true;
				ActivatePowerUp(powerup);
			}
		}
	}
}

bool Game::CheckCollision(GameObject& ball, GameObject& brick)
{
	bool collision_x = (ball.position.x + ball.size.x >= brick.position.x) && (brick.position.x + brick.size.x >= ball.position.x);
	bool collision_y = (ball.position.y + ball.size.y >= brick.position.y) && (brick.position.y + brick.size.y >= ball.position.y);
	return collision_x && collision_y;
}

Collision Game::CheckCollision(BallObject& ball, GameObject& brick)
{
	glm::vec2 center = ball.position + ball.radius;
	//	计算AABB信息（中心、半边长）
	glm::vec2 aabb_half = glm::vec2(brick.size.x / 2, brick.size.y / 2);
	glm::vec2 aabb_center = glm::vec2(brick.position.x + aabb_half.x, brick.position.y + aabb_half.y);

	//	两中心的矢量差
	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half, aabb_half);
	glm::vec2 closet_point = clamped + aabb_center;

	difference = closet_point - center;

	bool is_collision = glm::length(difference) < ball.radius;
	if (is_collision)
	{
		return Collision{ true, VectorDirection(difference), difference };
	}
	else
	{
		return Collision{ false, Direction::UP, glm::vec2(0.0f) };
	}
}

Direction Game::VectorDirection(glm::vec2 target)
{
	glm::vec2 compass[] = {
		glm::vec2(0.0f, 1.0f),  // 上
		glm::vec2(1.0f, 0.0f),  // 右
		glm::vec2(0.0f, -1.0f), // 下
		glm::vec2(-1.0f, 0.0f)  // 左
	};
	float max = 0.0f;
	unsigned int ball_direction = -1;
	for (unsigned i = 0; i < 4; i++)
	{
		float dot_result = glm::dot(compass[i], glm::normalize(target));
		if (dot_result > max)
		{
			max = dot_result;
			ball_direction = i;
		}
	}
	return static_cast<Direction>(ball_direction);
}
void Game::SpawnPowerUps(GameObject& brick)
{
	auto  ShouldSpawn = [](int chance) ->bool {return (rand() % chance) == 0; };

	if (ShouldSpawn(50))
	{
		this->powerups.emplace_back(ResourceManager::GetTexture("powerup_speed_texture"), "speed", 0.0f, brick.position, glm::vec3(0.5f, 0.5f, 1.0f));
	}
	if (ShouldSpawn(50))
	{
		this->powerups.emplace_back(ResourceManager::GetTexture("powerup_sticky_texture"), "sticky", 10.0f, brick.position, glm::vec3(1.0f, 0.5f, 1.0f));
	}
	if (ShouldSpawn(50))
	{
		this->powerups.emplace_back(ResourceManager::GetTexture("powerup_passthrough_texture"), "pass_through", 10.0f, brick.position, glm::vec3(0.5f, 0.0f, 0.0f));
	}
	if (ShouldSpawn(50))
	{
		this->powerups.emplace_back(ResourceManager::GetTexture("powerup_increase_texture"), "pad_increase", 0.0f, brick.position, glm::vec3(1.0f, 0.6f, 0.4f));
	}
	if (ShouldSpawn(50))
	{
		this->powerups.emplace_back(ResourceManager::GetTexture("powerup_confuse_texture"), "confuse", 15.0f, brick.position, glm::vec3(1.0f, 0.3f, 0.3f));
	}
	if (ShouldSpawn(50))
	{
		this->powerups.emplace_back(ResourceManager::GetTexture("powerup_chaos_texture"), "chaos", 15.0f, brick.position, glm::vec3(0.9f, 0.25f, 0.25f));
	}
}

void Game::ActivatePowerUp(PowerUp powerup)
{
	if (powerup.type == "speed")
	{
		this->ball->velocity *= 1.2f;
	}
	else if (powerup.type == "sticky")
	{
		std::cout << 1;
		this->ball->is_stick = true;
		this->player->color = glm::vec3(1.0f, 0.5f, 0.5f);
	}
	else if (powerup.type == "pass_through")
	{
		this->ball->is_pass_through = true;
		this->ball->color = glm::vec3(1.0f, 0.5f, 0.5f);;
	}
	else if (powerup.type == "pad_increase")
	{
		this->player->size.x += 50.0f;
	}
	else if (powerup.type == "confuse")
	{
		if (!this->post_processor->is_confuse)
		{
			this->post_processor->is_confuse = true;
		}
	}
	else if (powerup.type == "chaos")
	{
		if (!this->post_processor->is_chaos)
		{
			this->post_processor->is_chaos = true;
		}
	}
}

bool IsOtherPowerupActive(const std::vector<PowerUp>& powerups, const std::string& type)
{
	for (auto& powerup : powerups)
	{
		if (powerup.is_actived && powerup.type == type)
		{
			return true;
		}
	}
	return false;
}

void Game::UpdatePowerUps(float dt)
{
	for (auto& powerup : this->powerups)
	{
		powerup.position.y += powerup.velocity.y * dt;

		if (powerup.is_actived)
		{
			powerup.duration_time -= dt;
			if (powerup.duration_time <= 0.0f)
			{
				powerup.is_actived = false;
				if (powerup.type == "sticky")
				{
					if (!IsOtherPowerupActive(this->powerups, "sticky"))
					{
						this->ball->is_stick = false;
						this->player->color = glm::vec3(1.0f);
					}
				}
				else if (powerup.type == "pass_through")
				{
					if (!IsOtherPowerupActive(this->powerups, "pass_through"))
					{
						this->ball->is_pass_through = false;
						this->ball->color = glm::vec3(1.0f);
					}
				}
				else if (powerup.type == "confuse")
				{
					if (!IsOtherPowerupActive(this->powerups, "confuse"))
					{
						this->post_processor->is_confuse = false;
					}
				}
				else if (powerup.type == "chaos")
				{
					if (!IsOtherPowerupActive(this->powerups, "chaos"))
					{
						this->post_processor->is_chaos = false;
					}
				}
			}
		}
	}
	this->powerups.erase(std::remove_if(this->powerups.begin(), this->powerups.end(), [](const PowerUp& p) {return p.is_destory && !p.is_actived; }), this->powerups.end());
}