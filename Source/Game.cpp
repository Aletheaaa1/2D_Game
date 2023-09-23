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
	Level_2.Load("./Level/LevelTwo.txt", this->screen_width, this->screen_height / 2.0);
	Level_3.Load("./Level/LevelThree.txt", this->screen_width, this->screen_height / 2.0);
	Level_4.Load("./Level/LevelFour.txt", this->screen_width, this->screen_height / 2.0);
	this->levels.push_back(level_1);
	this->levels.push_back(Level_2);
	this->levels.push_back(Level_3);
	this->levels.push_back(Level_4);
	this->level = 1;

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

void Game::Update(float dt)
{
	ball->Move(dt, this->screen_width);
	this->DoCollision();
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
				if (this->ball->is_stack)
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
				if (this->ball->is_stack)
				{
					this->ball->position.x += -player_velocity;
				}
			}
		}
		if (this->keys[GLFW_KEY_SPACE])
		{
			this->ball->is_stack = false;
		}
	}
}

void Game::Render()
{
	this->renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(this->screen_width, this->screen_height), 0.0f);

	this->levels[this->level].Draw(*renderer);

	this->player->Draw(*renderer);

	this->ball->Draw(*renderer);
}

bool Game::CheckCollision(GameObject& ball, GameObject& brick)
{
	bool collision_x = (ball.position.x + ball.size.x >= brick.position.x) && (brick.position.x + brick.size.x >= ball.position.x);
	bool collision_y = (ball.position.y + ball.size.y >= brick.position.y) && (brick.position.y + brick.size.y >= ball.position.y);
	return collision_x && collision_y;
}

void Game::DoCollision()
{
	for (GameObject& brick : this->levels[this->level].bricks)
	{
		if (!brick.is_destory)
		{
			if (CheckCollision(*ball, brick))
			{
				if (brick.is_solid)
				{
					brick.is_destory = true;
				}
			}
		}
	}
}