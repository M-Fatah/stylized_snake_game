#pragma once

#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

#include "renderer.h"

struct Cell
{
	int x;
	int y;

	inline bool operator==(const Cell& rhs) const
	{
		return (this->x == rhs.x) && (this->y == rhs.y);
	}

	inline bool operator!=(const Cell& rhs) const
	{
		return !(*this == rhs);
	}
};

class Game_Object
{
public:
	
	Cell cell;
	
	glm::vec2 position;
	glm::vec2 scale;
	glm::vec4 color;
	
	Texture2D texture;

	Game_Object(Cell _cell, glm::vec2 _scale, glm::vec4 _color, Texture2D _texture);

	void draw_self(Renderer& renderer);
};