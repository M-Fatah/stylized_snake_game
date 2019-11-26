#pragma once

#include <GLAD/glad.h>
#include <glm/mat4x4.hpp>

#include <string>

class Shader
{
public:
	GLuint program_id;

	Shader() { }

	void use();
	void compile(const GLchar* vshader_source, const GLchar* fshader_soruce);
	void load_from_file(const char* vshader_path, const char* fshader_path);

	void set_uniform_float(const GLchar* name, GLfloat value);
	void set_uniform_integer(const GLchar* name, GLint value);
	void set_uniform_vector2f(const GLchar* name, GLfloat x, GLfloat y);
	void set_uniform_vector3f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z);
	void set_uniform_vector4f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	void set_uniform_vector4f(const GLchar* name, const glm::vec4& v4);
	void set_uniform_matrix4f(const GLchar* name, const glm::mat4& matrix);

private:

	void check_status(GLuint shader_id, bool is_program);
	std::string load_source_from_file(std::string path);
};