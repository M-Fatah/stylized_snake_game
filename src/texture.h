#pragma once

#include <glad/glad.h>

class Texture2D
{
public:
	GLuint id;

	GLuint width;
	GLuint height;

	GLuint mode_wrap_s;
	GLuint mode_wrap_t;

	GLuint mode_filter_min;
	GLuint mode_filter_mag;

	GLuint format_loaded;
	GLuint format_internal;
	
	GLfloat tiling_x;
	GLfloat tiling_y;

	Texture2D();
	
	void load_from_file(const char* file_path, bool generate_mipmaps);

	void bind();
};