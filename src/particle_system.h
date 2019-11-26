#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include "renderer.h"

// TODO: Refactor.
// TODO: Add configurable parameters to deploy different particle effects.
struct Particle
{
	glm::vec2 position;
	glm::vec2 scale;
	glm::vec2 velocity;
	glm::vec4 color;
	float rotation;
	float life_time;
};

class Particle_System
{
public:
	
	glm::vec2 position;
	glm::vec4 color;

	bool loop;
	float reset_after;

	unsigned int max_particles_num;
	unsigned int num_particles_per_frame;
	unsigned int next_particle_index;

	Particle particles[256];

	Texture2D particle_texture;

	Particle_System(glm::vec2 _position, Texture2D _particle_texture, glm::vec4 _color, unsigned int _max_particles_num, unsigned int _num_particles_per_frame, bool _loop = false);

	void update(float dt);
	
	void reset();

	void self_draw(Renderer& renderer);
};