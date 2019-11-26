#pragma once

#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "texture.h"

class Renderer
{
public:

	~Renderer();

	void init(float width, float height, Shader& _quad_shader);

	void clear(glm::vec4 color);

	void draw_quad(Texture2D& texture, glm::vec2 pos, glm::vec2 scale, glm::vec4 color);

	void draw_quad(Texture2D& texture, glm::vec2 pos, glm::vec2 scale, float rotation, glm::vec4 color);

	void draw_text(std::string text_to_draw, glm::vec2 pos, float scale, glm::vec4 color);

	void update_render_dimensions(int width, int height);

private:

	int render_width;
	int render_height;

	GLuint quad_vao;
	Shader quad_shader;
};