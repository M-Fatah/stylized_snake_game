#include "game.h"
#include "assets.h"

#include <ctime>
#include <string>


float ui_animation_dt = 0;

// TODO: Cleanup.
void Game::init(int width, int height, int tiles_count)
{
	// Load shaders from disk.
	Shader quad_shader;
	quad_shader.load_from_file("data/shaders/quad_vertex.glsl", "data/shaders/quad_fragment.glsl");

	Assets::shaders["quad_shader"] = quad_shader;
	
	// ----------------------------------------------------------

	// Load textures from disk.
	Texture2D tile_texture;
	tile_texture.mode_filter_min = GL_NEAREST_MIPMAP_NEAREST;
	tile_texture.load_from_file("data/textures/tile_0.png", true);

	Texture2D head_texture;
	head_texture.tiling_x = 2;
	head_texture.tiling_y = 2;
	head_texture.mode_filter_min = GL_LINEAR_MIPMAP_LINEAR;
	head_texture.load_from_file("data/textures/tile_0.png", true);
	
	Assets::textures["tile"] = tile_texture;
	Assets::textures["head"] = head_texture;

	// ----------------------------------------------------------

	// Load audio clips from disk.
	audio = new Audio();

	//Assets::audio_clips["eat"] = audio->load_wav_from_file("data/audio/big_bite.wav");
	Assets::audio_clips["eat"] = audio->load_ogg_from_file("data/audio/big_bite.ogg");
	Assets::audio_sources["sfx_source"] = audio->create_audio_source(0.75f, 1.f);

	//Audio_Clip bg_clip = Assets::audio_clips["bg"] = audio->load_wav_from_file("data/audio/bg.wav");
	//Audio_Source music_source = Assets::audio_sources["music_source"] = audio->create_audio_source(0.25f, 2.f);
	Audio_Clip bg_clip = Assets::audio_clips["bg"] = audio->load_ogg_from_file("data/audio/bg.ogg");
	Audio_Source music_source = Assets::audio_sources["music_source"] = audio->create_audio_source(0.10f, 1.f);
	
	audio->play(music_source, bg_clip, true);

	// ----------------------------------------------------------

	// Init renderer.
	renderer = new Renderer();
	renderer->init(width, height, quad_shader);
	
	// Set members.
	this->width  = width;
	this->height = height;
	this->aspect = static_cast<float>(height) / width;
	
	this->tile_count_x = tiles_count;
	this->tile_count_y = this->tile_count_x * aspect;

	cell_width  = (static_cast<float>(width)  / this->tile_count_x);
	cell_height = (static_cast<float>(height) / this->tile_count_y);
	
	current_direction = DIRECTION::RIGHT;
	old_direction     = current_direction;

	// Snake head.
	snake_current_cell = { 3, 3 };
	snake_last_cell    = snake_current_cell;

	// Fill empty tiles at start.
	for (int y = 1; y <= this->tile_count_y; y++)
	{
		for (int x = 1; x <= this->tile_count_x; x++)
		{	
			Cell c = { x , y };
			
			if (c != snake_current_cell)
				empty_cells.push_back(c);
		}
	}

	// Create snake head.
	Game_Object snake_head = Game_Object(snake_current_cell, { cell_width * 1.1f, cell_height * 1.1f }, { 0.0f, 1.0f, 0.2f, 1.0f }, head_texture);
	snake_head.position = glm::vec2(snake_current_cell.x * cell_width - cell_width * 0.5f, snake_current_cell.y * cell_height - cell_height * 0.5f);
	snake.push_back(snake_head);

	// Add one tail part to the snake.
	Cell snake_part_cell = { 2, 3 };
	Game_Object snake_part = Game_Object(snake_part_cell, snake[0].scale * 0.85f, { 0.0f, 1.0f, 0.2f, 1.0f }, tile_texture);
	snake_part.position = glm::vec2(snake_part_cell.x * cell_width - cell_width * 0.5f, snake_part_cell.y * cell_height - cell_height * 0.5f);
	snake.push_back(snake_part);

	// Init prng with a different seed each time.
	srand(static_cast<unsigned int>(time(0)));
	
	// @Cleanup.
	// Apple.
	apple = new Game_Object({ 2, 2 }, { cell_width * 0.75f, cell_height * 0.75f }, { 0.3f, 1.0f, 0.05f, 1.0f }, tile_texture);
	apple->cell = empty_cells[rand() % empty_cells.size()];
	apple->position = glm::vec2(apple->cell.x * cell_width - cell_width * 0.5f, apple->cell.y * cell_height - cell_height * 0.5f);
	
	game_paused = false;
	game_over = false;

	// Particle system.
	apple_particles = new Particle_System(apple->position, tile_texture, apple->color, 32, 32, false);

	// TODO: Clean up.
	// Rock particles must be created before spawning a rock at game start to prevent it from accessing garbage memory.
	rocks_particles = new Particle_System(apple->position, tile_texture, { 0.3f, 0.3f, 0.3f, 1.0f }, 32, 32, false);
	
	// Rock.
	spawn_rock();
}

// Spawn a rock at a random position each time the snake eats an apple.
void Game::spawn_rock()
{
	// Get the rock texture.
	Texture2D tex = Assets::get_texture("tile");
	tex.tiling_x = 1 + (rand() % 2);
	tex.tiling_y = 1 + (rand() % 2);

	// Get a random empty cell.
	Cell c = empty_cells[rand() % empty_cells.size()];
	
	// Create a rock game_object.
	Game_Object rock = Game_Object(c, { cell_width * 0.75f, cell_height * 0.75f }, { 0.6f, 0.6f, 0.6f, 1.0f }, tex);
	rock.position = glm::vec2(c.x * cell_width - cell_width * 0.5f, c.y * cell_height - cell_height * 0.5f);
	
	// Add the rock to the list of already spawn rocks.
	rocks.push_back(rock);
	
	// Remove the rock's cell from empty cells list.
	remove_from_empty_cells(c);
	
	// Spawn particles.
	rocks_particles->position = rock.position;
	rocks_particles->reset();
}

// TODO: @Clenaup @Redundant as Game::init() function.
void Game::restart()
{
	snake.clear();
	empty_cells.clear();
	rocks.clear();

	current_direction = DIRECTION::RIGHT;
	old_direction = current_direction;

	// Snake head.
	snake_current_cell = { 3, 3 };
	snake_last_cell = snake_current_cell;

	// Fill empty tiles at start.
	for (int y = 1; y <= this->tile_count_y; y++)
	{
		for (int x = 1; x <= this->tile_count_x; x++)
		{
			Cell c = { x , y };

			if (c != snake_current_cell)
				empty_cells.push_back(c);
		}
	}

	// Create snake head.
	Texture2D head_texture = Assets::get_texture("head");
	Game_Object snake_head = Game_Object(snake_current_cell, { cell_width * 1.1f, cell_height * 1.1f }, { 0.0f, 1.0f, 0.2f, 1.0f }, head_texture);
	snake_head.position = glm::vec2(snake_current_cell.x * cell_width - cell_width * 0.5f, snake_current_cell.y * cell_height - cell_height * 0.5f);
	snake.push_back(snake_head);

	// Add one tail part to the snake.
	Texture2D tile_texture = Assets::get_texture("tile");
	Cell snake_part_cell = { 2, 3 };
	Game_Object snake_part = Game_Object(snake_part_cell, snake[0].scale * 0.85f, { 0.0f, 1.0f, 0.2f, 1.0f }, tile_texture);
	snake_part.position = glm::vec2(snake_part_cell.x * cell_width - cell_width * 0.5f, snake_part_cell.y * cell_height - cell_height * 0.5f);
	snake.push_back(snake_part);

	srand(static_cast<unsigned int>(time(0)));
	
	// Rock.
	spawn_rock();

	// Apple.
	apple->cell = empty_cells[rand() % empty_cells.size()];
	apple->position = glm::vec2(apple->cell.x * cell_width - cell_width * 0.5f, apple->cell.y * cell_height - cell_height * 0.5f);

	// Particle system.
	apple_particles->position = apple->position;
	apple_particles->reset();

	game_paused = false;
	game_over = false;

	score = 0;
	snake_speed = 8.0f;
}

void Game::process_input()
{
	if (keyboard_keys[KEY_P])
	{
		if(!game_over)
			game_paused = !game_paused;
	}

	if (keyboard_keys[KEY_R])
	{
		restart();
	}

	if (keyboard_keys[KEY_LEFT_BRACKET])
	{
		snake_speed--;
		if (snake_speed < 0)
			snake_speed = 0;
	}
	else if (keyboard_keys[KEY_RIGHT_BRACKET])
		snake_speed++;

	if (!game_paused)
	{
		if (keyboard_keys[KEY_W] || keyboard_keys[KEY_UP])
		{
			if (old_direction == DIRECTION::LEFT || old_direction == DIRECTION::RIGHT)
				current_direction = DIRECTION::UP;
		}
		else if (keyboard_keys[KEY_S] || keyboard_keys[KEY_DOWN])
		{
			if (old_direction == DIRECTION::LEFT || old_direction == DIRECTION::RIGHT)
					current_direction = DIRECTION::DOWN;
		}
		else if (keyboard_keys[KEY_A] || keyboard_keys[KEY_LEFT])
		{
			if (old_direction == DIRECTION::UP || old_direction == DIRECTION::DOWN)
					current_direction = DIRECTION::LEFT;
		}
		else if (keyboard_keys[KEY_D] || keyboard_keys[KEY_RIGHT])
		{
			if (old_direction == DIRECTION::UP || old_direction == DIRECTION::DOWN)
					current_direction = DIRECTION::RIGHT;
		}
	}
}

/// <summary>
/// Removes the cell passed as a parameter from empty_cells vector. 
/// </summary>
void Game::remove_from_empty_cells(Cell cell)
{
	// TODO: Optimize.

	// Remove the current snake head cell from the empty list.
	// Finding the element by value.
	auto it = std::find(empty_cells.begin(), empty_cells.end(), cell);
	if (it != empty_cells.end())
	{
		// Swap the element with the back of the empty_cells vector then pop it. (Avoiding overhead of incrementing succeeding elements in the vector)
		std::swap(*it, empty_cells.back());
		empty_cells.pop_back();
	}
}

void Game::update(float dt)
{
	// TODO: Clean up.
	static float a = dt;
	a += dt * 2;

	ui_animation_dt = abs(sin(a)) - 0.5f;

	if (!game_paused)
	{
		step_timer += dt * snake_speed;

		if (step_timer >= 1.f)
		{
			step_timer = 0;

			snake_last_cell = snake_current_cell;

			switch (current_direction)
			{
				case DIRECTION::UP:
				{
					snake_current_cell.y += 1;
				} break;

				case DIRECTION::DOWN:
				{
					snake_current_cell.y -= 1;
				} break;

				case DIRECTION::LEFT:
				{
					snake_current_cell.x -= 1;
				} break;

				case DIRECTION::RIGHT:
				{
					snake_current_cell.x += 1;
				} break;
			}

			// Add last cell used by the tail to the empty list.
			Cell last_tail_cell = snake[snake.size() - 1].cell;
			empty_cells.push_back(last_tail_cell);

			// Move snake parts.
			for (int i = snake.size() - 1; i > 0; i--)
			{
				snake[i].cell = snake[i - 1].cell;
				snake[i].position = snake[i - 1].position;
			}

			// Move snake head.
			if (snake_current_cell.x <= 0)
				snake_current_cell.x = tile_count_x;
			else if (snake_current_cell.x > tile_count_x)
				snake_current_cell.x = 1;

			if (snake_current_cell.y <= 0)
				snake_current_cell.y = tile_count_y;
			else if (snake_current_cell.y > tile_count_y)
				snake_current_cell.y = 1;

			snake[0].cell = snake_current_cell;
			snake[0].position = glm::vec2(snake_current_cell.x * cell_width - cell_width * 0.5f, snake_current_cell.y * cell_height - cell_height * 0.5f);

			remove_from_empty_cells(snake[0].cell);

			old_direction = current_direction;
		}

		// Check if snake hit one of its tail parts then game over!
		for (int i = 1; i < snake.size(); i++)
		{
			if (snake_current_cell == snake[i].cell)
			{
				// TODO: GAME OVER!
				// TODO: Cleanup.
				snake_current_cell = snake_last_cell;

				snake_speed = 0;
				game_over = true;
			}
		}

		// Collision detection with rocks.
		for (unsigned int i = 0; i < rocks.size(); ++i)
		{
			if (snake[0].cell == rocks[i].cell)
			{
				// TODO: GAME OVER!
				snake_current_cell = snake_last_cell;

				snake_speed = 0;

				game_over = true;
			}
		}

		// Collision detection with the apple.
		if (snake[0].cell == apple->cell)
		{
			// Play sound.
			Audio_Clip clip = Assets::audio_clips["eat"];
			Audio_Source source = Assets::audio_sources["sfx_source"];
			audio->play(source, clip);

			// Increase score.
			score += 10;

			// Increase game speed.
			snake_speed += 0.3f;

			// Spawn a rock 50% chance.
			if (rand() % 100 + 1 > 50)
				spawn_rock();

			// Grow tail.
			{
				// Get the last occupied tile to place the new added tail element on it.
				Cell c_offset = snake_last_cell;
				if (snake.size() > 1)
				{
					Cell c1 = snake[snake.size() - 1].cell;
					Cell c2 = snake[snake.size() - 2].cell;
					c_offset = { 2 * c1.x - c2.x, 2 * c1.y - c2.y };
					
					// Make sure cell fit within our map boundaries.
					// The last tail's cell is positioned relative to the previous last tail's cell.
					if (c_offset.x <= 0)
						c_offset.x = tile_count_x - c1.x;
					else if (c_offset.x > tile_count_x)
						c_offset.x = c1.x + 1;

					if (c_offset.y <= 0)
						c_offset.y = tile_count_y - c1.y;
					else if (c_offset.y > tile_count_y)
						c_offset.y = c1.y + 1;
				}

				// Add the new tail tile to the end of snake vector.
				Texture2D snake_body_tex = Assets::get_texture("tile");
				Game_Object tail = Game_Object(c_offset, snake[0].scale * 0.85f, glm::vec4(1.0f), snake_body_tex);
				tail.position = glm::vec2(tail.cell.x * cell_width - cell_width * 0.5f, tail.cell.y * cell_height - cell_height * 0.5f);
				snake.push_back(tail);

				remove_from_empty_cells(tail.cell);
			}

			// Respawn the apple.
			{			
				// Get random tile & place the apple.
				apple->cell = empty_cells[rand() % empty_cells.size()];
				apple->position = glm::vec2(apple->cell.x * cell_width - cell_width * 0.5f, apple->cell.y * cell_height - cell_height * 0.5f);

				// Spawn the particle system.
				apple_particles->position = apple->position;
				apple_particles->reset();
			}
		}
	}

	// Update particle system.
	apple_particles->update(dt);
	rocks_particles->update(dt);

	// TODO: Winning condition.
	if (empty_cells.size() < 1)
		game_paused = true;
	
	// TODO: Cleanup.
	// TODO: Create another array to hold the last state the button was in and compare with to check if its down or held.
	// TODO: Use vector of the last pressed keys and reset them?!?!?!
	// Clear input of each supported key to false for the next frame.
	for (auto& i : supported_keys)
		keyboard_keys[i] = false;
}

void Game::render()
{
	// Clear the screen with solid color.
	renderer->clear(glm::vec4(0.0f));

	// Draw background tiles.
	Texture2D tex = Assets::get_texture("tile");
	tex.tiling_x = tile_count_x;
	tex.tiling_y = tile_count_y;

	renderer->draw_quad(tex, { this->width * 0.5f, this->height * 0.5f }, { this->width, this->height }, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

	// Drawing the snake parts backwards to make the head appear at top.
	for (int i = snake.size() - 1; i >= 0; i--)
	{
		float percent = static_cast<float>(i) / static_cast<float>(snake.size());
		glm::vec3 c = glm::mix(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), percent);

		snake[i].color = glm::vec4(c.r, c.g, c.b, 1.0f);
		snake[i].draw_self(*renderer);
	}

	// Draw particle system.
	apple_particles->self_draw(*renderer);
	rocks_particles->self_draw(*renderer);

	// Draw apple.
	apple->draw_self(*renderer);

	// Draw rocks.
	for (unsigned int i = 0; i < rocks.size(); ++i)
		rocks[i].draw_self(*renderer);

	// Draw score.
	renderer->draw_text(std::to_string(score), { this->width * 0.5f, 50 }, 3.0f + ui_animation_dt, glm::vec4(0.3, 0.7f, 0.9f, 1.0f));

	// Draw game paused text.
	if (game_paused)
	{
		renderer->draw_text("GAME PAUSED!", { this->width * 0.5f, this->height * 0.5f - 40 }, 5.0f + ui_animation_dt * .25f, glm::vec4(0.5, 0.8f, 0.2f, 1.0f));
		renderer->draw_text("Press [P] to unpause.", { this->width * 0.5f, this->height * 0.5f + 30 }, 2.0f + ui_animation_dt * .25f, glm::vec4(1.0, 0.8f, 0.2f, 1.0f));
	}

	// Draw game over text.
	if (game_over)
	{
		renderer->draw_text("GAME OVER!", { this->width * 0.5f, this->height * 0.5f - 40 }, 5.0f + ui_animation_dt * .25f, glm::vec4(1.0, 0.1f, 0.1f, 1.0f));
		renderer->draw_text("Press [R] to restart the game.", { this->width * 0.5f, this->height * 0.5f + 30 }, 3.0f + ui_animation_dt * .25f, glm::vec4(0.5, 0.8f, 0.2f, 1.0f));
	}
}

void Game::update_dimensions(int width, int height)
{
	// Checks if the game window is minimized and if so, pause the game.
	if (width == 0 || height == 0)
	{
		if (!game_over)
		{
			game_paused = true;
			return;
		}
	}

	this->width = width;
	this->height = height;
	this->aspect = static_cast<float>(height) / static_cast<float>(width);

	cell_width  = (static_cast<float>(width)  / tile_count_x);
	cell_height = (static_cast<float>(height) / tile_count_y);

	// Rescale snake.
	for (unsigned int i = 0; i < snake.size(); ++i)
	{
		Game_Object* snake_part = &snake[i];

		if (i == 0)
			snake_part->scale = { cell_width * 1.1f, cell_height * 1.1f };
		else
			snake_part->scale = { cell_width * 0.9f, cell_height * 0.9f };
		
		snake_part->position = glm::vec2(snake_part->cell.x * cell_width - cell_width * 0.5f, snake_part->cell.y * cell_height - cell_height * 0.5f);
	}

	// Rescale apple.
	apple->scale = { cell_width * 0.75f, cell_height * 0.75f };
	apple->position = glm::vec2(apple->cell.x * cell_width - cell_width * 0.5f, apple->cell.y * cell_height - cell_height * 0.5f);

	// Rescale rocks.
	for (unsigned int i = 0; i < rocks.size(); ++i)
	{
		Game_Object* rock = &rocks[i];
		rock->scale = { cell_width * 0.75f, cell_height * 0.75f };
		rock->position = glm::vec2(rock->cell.x * cell_width - cell_width * 0.5f, rock->cell.y * cell_height - cell_height * 0.5f);
	}

	// Rescale particle system.
	apple_particles->position = apple->position;
	rocks_particles->position = rocks[0].position;

	renderer->update_render_dimensions(width, height);
}

void Game::terminate()
{
	delete apple;
	delete renderer;
	delete audio;
	delete rocks_particles;
	delete apple_particles;

	Assets::dispose();
}