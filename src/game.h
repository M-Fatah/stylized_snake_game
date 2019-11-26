#pragma once

#include "renderer.h"
#include "game_object.h"
#include "particle_system.h"
#include "audio.h"

#include <vector>

#include <glfw/glfw3.h>


enum class DIRECTION
{
	UP, DOWN, LEFT, RIGHT
};

enum SUPPORTED_KEYS
{
	KEY_W             = GLFW_KEY_W, 
	KEY_S             = GLFW_KEY_S, 
	KEY_A             = GLFW_KEY_A, 
	KEY_D             = GLFW_KEY_D, 
	KEY_P             = GLFW_KEY_P, 
	KEY_R             = GLFW_KEY_R, 
	KEY_UP            = GLFW_KEY_UP,
	KEY_DOWN          = GLFW_KEY_DOWN,
	KEY_LEFT          = GLFW_KEY_LEFT,
	KEY_RIGHT         = GLFW_KEY_RIGHT,
	KEY_LEFT_BRACKET  = GLFW_KEY_LEFT_BRACKET,
	KEY_RIGHT_BRACKET = GLFW_KEY_RIGHT_BRACKET
};

class Game
{
public:

	bool keyboard_keys[1024];

	void init(int width, int height, int tiles_count);

	void terminate();
	
	void process_input();

	void update(float dt);

	void render();

	void update_dimensions(int width, int height);

	void remove_from_empty_cells(Cell cell);

	void spawn_rock();

	void restart();

private:

	int width;
	int height;
	int tile_count_x;
	int tile_count_y;

	bool game_paused;
	bool game_over;
	
	float aspect;
	float cell_width;
	float cell_height;

	float step_timer   = 0;
	float snake_speed  = 8.0f;
	
	unsigned int score = 0;
	
	std::vector<Cell>        empty_cells;
	std::vector<Game_Object> snake;
	std::vector<Game_Object> rocks;

	Cell      snake_last_cell;
	Cell      snake_current_cell;
	DIRECTION old_direction;
	DIRECTION current_direction;
	
	Renderer*        renderer;
	Game_Object*     apple;
	Audio*           audio;
	Particle_System* apple_particles;
	Particle_System* rocks_particles;

	int supported_keys[12] = { KEY_UP , KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_W, KEY_S, KEY_A, KEY_D, KEY_P, KEY_R, KEY_LEFT_BRACKET, KEY_RIGHT_BRACKET };
};