#include "Game.h"

Game::Game(unsigned int screen_width, unsigned int screen_height) :state(GameState::GAME_ACTIVE), screen_width(screen_width), screen_height(screen_width), keys()
{
}

void Game::Init()
{
	Shader shader = ResourceManager::LoadShader("./Shaders/sprite.vs", "./Shaders/sprite.fs", "", "sprite_shader");

	Texture background_texture = ResourceManager::LoadTexture("./Textures/background.jpg", true, false, "background");
	Texture block_texture = ResourceManager::LoadTexture("./Textures/block.png", true, false, "block");
	Texture block_solid_texture = ResourceManager::LoadTexture("./Textures/block_solid.png", true, false, "block_solid");
	Texture player_texture = ResourceManager::LoadTexture("./Textures/paddle.png", true, true, "player");
	Texture ball_texture = ResourceManager::LoadTexture("./Textures/awesomeface.png", true, false, "ball");

	glm::mat4 projection = glm::ortho(0.0f, float(this->screen_width), float(this->screen_height), 0.0f, -0.0f, 1.0f);
	shader.Bind();
	shader.SetUniformMat4("projection", projection);
	shader.SetUniform1i("image_texture", 0);

	this->renderer = std::make_unique<SpriteRenderer>(shader);

	//
	//	Level
	//
	GameLevel level_1, Level_2, Level_3, Level_4;
	level_1.Load("./Level/LevelOne.txt", this->screen_width, this->screen_height / 2.0);
	//Level_2.Load("./Level/LevelTwo.txt", this->screen_width, this->screen_height / 2.0);
	//Level_3.Load("./Level/LevelThree.txt", this->screen_width, this->screen_height / 2.0);
	//Level_4.Load("./Level/LevelFour.txt", this->screen_width, this->screen_height / 2.0);
	this->levels.push_back(level_1);
	//this->levels.push_back(Level_2);
	//this->levels.push_back(Level_3);
	//this->levels.push_back(Level_4);
	this->level = 0;

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
}

void Game::Render()
{
	this->renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(this->screen_width, this->screen_height), 0.0f);

	this->levels[this->level].Draw(*renderer);

	this->player->Draw(*renderer);

	this->ball->Draw(*renderer);
}

void Game::Update(float dt)
{
	ball->Move(dt, this->screen_width);
	this->DoCollision();

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

	player->position = glm::vec2(screen_width / 2.0f - PLAYER_SIZE.x / 2.0f, screen_height - PLAYER_SIZE.y);
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
				}
				Direction direction = collision.direction;
				glm::vec2 distance = collision.distance;
				//	碰撞反转小球
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
}

//bool Game::CheckCollision(GameObject& ball, GameObject& brick)
//{
//	bool collision_x = (ball.position.x + ball.size.x >= brick.position.x) && (brick.position.x + brick.size.x >= ball.position.x);
//	bool collision_y = (ball.position.y + ball.size.y >= brick.position.y) && (brick.position.y + brick.size.y >= ball.position.y);
//	return collision_x && collision_y;
//}

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