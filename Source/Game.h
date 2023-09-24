#pragma once
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <memory>

#include "../Headers/ResourceManager.h"
#include "SpriteRenderer.h"
#include "GameLevel.h"
#include "GameObject.h"
#include "BallObject.h"

constexpr glm::vec2 PLAYER_SIZE{ 100.0f, 20.0f };
constexpr glm::vec2 INITIAL_BALL_VELOCITY{ -100.0f, -350.0f };
constexpr float PLAYER_VELOCITY{ 500.0f };
constexpr float BALL_VELOCITY{ 700.0f };
constexpr float BALL_RADIUS{ 12.5f };
constexpr glm::vec2 BALL_SIZE{ glm::vec2(2 * BALL_RADIUS) };

enum class GameState
{
	GAME_ACTIVE = 0,
	GAME_MENIU,
	GAME_WIN,
};

enum class Direction
{
	UP = 0,
	RIGHT,
	DOWN,
	LEFT,
};

struct Collision
{
	bool is_collision;
	Direction direction;
	glm::vec2 distance;
};

class Game
{
public:
	GameState state;
	bool keys[1024];
	unsigned int screen_width;
	unsigned int screen_height;
	std::unique_ptr<SpriteRenderer> renderer;
	std::unique_ptr<GameObject> player;
	std::unique_ptr<BallObject> ball;

	unsigned int level;
	std::vector<GameLevel> levels;

public:
	Game(unsigned int screen_width, unsigned int screen_height);
	void ProcessInput(float dt);
	void Update(float dt);
	void Init();
	void Render();
	Collision CheckCollision(BallObject& ball, GameObject& brick);
	void DoCollision();
	//	重定向
	Direction VectorDirection(glm::vec2 target);
	//	重置
	void ResetObject();
	void ResetLevel();
};
