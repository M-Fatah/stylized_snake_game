#include "renderer.h"

#include <iostream>

#define GLT_IMPLEMENTATION
#define GLT_MANUAL_VIEWPORT
#include "ext/gltext.h"

void Renderer::init(float width, float height, Shader& _quad_shader)
{
	// Quad vertices.
	GLfloat quad_vertices[] =
	{
		// Anti-clockwise.
		// Position       // Texture coords.
		  -0.5f, -0.5f,      0.0f, 0.0f,
		   0.5f, -0.5f,      1.0f, 0.0f,
		   0.5f,  0.5f,      1.0f, 1.0f,
		  			      
		  -0.5f, -0.5f,      0.0f, 0.0f,
		   0.5f,  0.5f,      1.0f, 1.0f,
		  -0.5f,  0.5f,      0.0f, 1.0f
	};

	// Generate VAO for quads.
	glGenVertexArrays(1, &quad_vao);
	glBindVertexArray(quad_vao);

	// Generate VBO for quads.
	GLuint quad_vbo;
	glGenBuffers(1, &quad_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

	// Vertex attribute configuration for quad.
	// Vertex position.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
	// Vertex texture coords.
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

	// Unbind quads buffers.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// =================================================================== //
	
	// OpenGL config.
	glEnable(GL_CULL_FACE);
	
	// Blending.
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Depth mask.
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);
	
	// =================================================================== //
	
	// Init text rendering.
	gltInit();

	// =================================================================== //

	this->quad_shader   = _quad_shader;
	this->render_width  = width;
	this->render_height = height;

	update_render_dimensions(width, height);
}

void Renderer::update_render_dimensions(int width, int height)
{
	glViewport(0, 0, width, height);

	// NOTE: gltViewport throus an error due to an assert done on width and height being 0 if the game window is minimized.
	//		 I surrounded it with if conditions to avoid editing the original gltext.h file.
	if (width > 0 && height > 0)
	{
		gltViewport(width, height);
	}

	this->render_width = width;
	this->render_height = height;

	glm::mat4 projection = glm::ortho<float>(0.0f, width, 0.0f, height, -1.0f, 1.0f);
	this->quad_shader.use();
	this->quad_shader.set_uniform_matrix4f("projection", projection);
}

void Renderer::clear(glm::vec4 color)
{
	glClearDepth(1.0f);
	glClearColor(color.x, color.y, color.z, color.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::draw_quad(Texture2D& texture, glm::vec2 pos, glm::vec2 scale, glm::vec4 color)
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glm::mat4 projection = glm::ortho<float>(0.0f, this->render_width, 0.0f, this->render_height, -1.0f, 1.0f);
	glm::mat4 model_matrix(1.0f);
	model_matrix = glm::translate(model_matrix, glm::vec3(pos.x, pos.y, 0.0f));
	model_matrix = glm::scale(model_matrix, glm::vec3(scale.x, scale.y, 1.0f));
	
	
	this->quad_shader.use();
	this->quad_shader.set_uniform_matrix4f("model", model_matrix);
	this->quad_shader.set_uniform_matrix4f("projection", projection);
	this->quad_shader.set_uniform_vector2f("tiling", texture.tiling_x, texture.tiling_y);
	this->quad_shader.set_uniform_vector4f("color", color.x, color.y, color.z, color.w);

	glActiveTexture(GL_TEXTURE0);
	texture.bind();

	// TODO: Use index buffers to render more efficiently.
	glBindVertexArray(this->quad_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Renderer::draw_quad(Texture2D& texture, glm::vec2 pos, glm::vec2 scale, float rotation, glm::vec4 color)
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glm::mat4 projection = glm::ortho<float>(0.0f, this->render_width, 0.0f, this->render_height, -1.0f, 1.0f);
	glm::mat4 model_matrix(1.0f);
	model_matrix = glm::translate(model_matrix, glm::vec3(pos.x, pos.y, 0.0f));
	model_matrix = glm::rotate(model_matrix, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
	model_matrix = glm::scale(model_matrix, glm::vec3(scale.x, scale.y, 1.0f));

	this->quad_shader.use();
	this->quad_shader.set_uniform_matrix4f("model", model_matrix);
	this->quad_shader.set_uniform_matrix4f("projection", projection);
	this->quad_shader.set_uniform_vector2f("tiling", texture.tiling_x, texture.tiling_y);
	this->quad_shader.set_uniform_vector4f("color", color.x, color.y, color.z, color.w);

	glActiveTexture(GL_TEXTURE0);
	texture.bind();

	// TODO: Use index buffers to render more efficiently.
	glBindVertexArray(this->quad_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

// TODO: Use manual view_port settings?!
void Renderer::draw_text(std::string text_to_draw, glm::vec2 pos, float scale, glm::vec4 color)
{
	// Creating text
	GLTtext* text = gltCreateText();
	gltSetText(text, text_to_draw.c_str());

	// Begin text drawing (this for instance calls glUseProgram)
	gltBeginDraw();

	// Draw any amount of text between begin and end
	gltColor(color.x, color.y, color.z, color.w);
	gltDrawText2DAligned(text, pos.x, pos.y, scale, GLT_CENTER, GLT_CENTER);

	// Finish drawing text
	gltEndDraw();

	// Deleting text
	gltDeleteText(text);
}

Renderer::~Renderer()
{
	glDeleteVertexArrays(1, &this->quad_vao);
	gltTerminate();
}