#include "particle_system.h"

Particle_System::Particle_System(glm::vec2 _position, Texture2D _particle_texture, glm::vec4 _color, unsigned int _max_particles_num, unsigned int _num_particles_per_frame, bool _loop)
{
	loop = _loop;
	position = _position;
	color = _color;
	particle_texture = _particle_texture;
	max_particles_num = _max_particles_num;
	num_particles_per_frame = _num_particles_per_frame;
	reset_after = 0;


	reset();
}

void Particle_System::update(float dt)
{
	for (unsigned int i = 0; i < max_particles_num; ++i)
	{
		Particle* p = &particles[i];
		p->position += p->velocity * dt;
		p->color = glm::mix(p->color, glm::vec4(0.0f), dt * (1.0f / p->life_time));
		p->rotation += 5 * dt;
		p->life_time -= dt;
	}


	if (next_particle_index >= max_particles_num)
		reset_after += dt;
	
	for (unsigned int i = 0; i < num_particles_per_frame; ++i)
	{
		if (next_particle_index >= max_particles_num)
		{
			if (loop)
			{		
				if (reset_after >= 1.0f)
				{
					reset();
					reset_after = 0;
				}
				else
				{
					return;
				}
			}
			else
			{
				return;
			}
		}

		Particle* p = &particles[next_particle_index++];
		p->position = this->position;
		p->scale    = glm::vec2((rand() % 10 - 5) + 10);
		p->velocity = glm::vec2((rand() % 150) - 75, (rand() % 150) - 75);
		p->color    = this->color;
		p->life_time = 1.25f;
	}
}

void Particle_System::reset()
{
	next_particle_index = 0;
	reset_after = 0;

	for (unsigned int i = 0; i < max_particles_num; ++i)
	{
		Particle* p = &particles[i];
		p->position = this->position;
		p->scale = glm::vec2(25);
		p->velocity = glm::vec2(5.0f, 20.0f);
		p->color = glm::vec4(1.0f);
		p->rotation = 0;
		p->life_time = 1.25f;
	}
}

void Particle_System::self_draw(Renderer& renderer)
{
	for (unsigned int i = 0; i < max_particles_num; ++i)
	{
		renderer.draw_quad(particle_texture, particles[i].position, particles[i].scale, particles[i].rotation, particles[i].color);
	}
}