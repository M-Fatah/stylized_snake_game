#include "texture.h"


#define STB_IMAGE_IMPLEMENTATION
#include "ext/stb_image.h"

Texture2D::Texture2D() : width(1), height(1), format_internal(GL_RGBA), format_loaded(GL_RGBA), mode_wrap_s(GL_REPEAT), mode_wrap_t(GL_REPEAT), mode_filter_min(GL_NEAREST), mode_filter_mag(GL_LINEAR), tiling_x(1), tiling_y(1)
{
	// Generate a texture and assign it to this id.
	glGenTextures(1, &this->id);
}

void Texture2D::load_from_file(const char* file_path, bool generate_mipmaps = false)
{
	// Flip image vertically. so the origin starts from bottom-left.
	stbi_set_flip_vertically_on_load(true);

	// Load image data.
	int _width, _height, _num_of_channels;
	GLubyte* data = stbi_load(file_path, &_width, &_height, &_num_of_channels, STBI_rgb_alpha);

	this->width  = _width;
	this->height = _height;

	// TODO: Set format based on _num_of_channels.
	// TODO: Figure out why _num_of_channels return 2.
	//if (_num_of_channels <= 3)
	//{
	//	format_loaded   = GL_RGBA;
	//	format_internal = GL_RGB8;
	//}

	// Bind texture.
	glBindTexture(GL_TEXTURE_2D, this->id);

	// Set texture parameters.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->mode_wrap_s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->mode_wrap_t);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->mode_filter_mag);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->mode_filter_min);

	// Generate texture from loaded data.
	glTexImage2D(GL_TEXTURE_2D, 0, this->format_internal, this->width, this->height, 0, this->format_loaded, GL_UNSIGNED_BYTE, data);
	
	// Generate mipmaps.
	if(generate_mipmaps)
		glGenerateMipmap(GL_TEXTURE_2D);

	// Unbind texture.
	glBindTexture(GL_TEXTURE_2D, 0);

	// Free image data.
	stbi_image_free(data);
}

void Texture2D::bind()
{
	glBindTexture(GL_TEXTURE_2D, this->id);
}