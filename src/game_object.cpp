#include "game_object.h"

Game_Object::Game_Object(Cell _cell, glm::vec2 _scale, glm::vec4 _color, Texture2D _texture) : cell(_cell), scale(_scale), color(_color), texture(_texture)
{
	position.x = cell.x * scale.x - scale.x * 0.5f;
	position.y = cell.y * scale.y - scale.y * 0.5f;
}

void Game_Object::draw_self(Renderer& renderer)
{
	renderer.draw_quad(this->texture, this->position, this->scale, this->color);
}