#include "shader.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <glm/gtc/type_ptr.hpp>

void Shader::load_from_file(const char* vshader_path, const char* fshader_path)
{
	std::string vshader_source = load_source_from_file(vshader_path);
	std::string fshader_source = load_source_from_file(fshader_path);

	compile(vshader_source.c_str(), fshader_source.c_str());
}

std::string Shader::load_source_from_file(std::string path)
{
	std::string txt;

	try
	{
		std::ifstream fstream(path);

		std::stringstream sstream;

		sstream << fstream.rdbuf();

		fstream.close();

		txt = sstream.str();
	}
	catch (std::exception e)
	{
		std::cerr << "Error: reading file faild, " << e.what() << std::endl;
	}

	return txt;
}

void Shader::compile(const GLchar* vshader_source, const GLchar* fshader_source)
{
	// Vertex shader.
	GLuint vshader;
	vshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vshader, 1, &vshader_source, NULL);
	glCompileShader(vshader);
	check_status(vshader, false);

	// Fragment shader.
	GLuint fshader;
	fshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fshader, 1, &fshader_source, NULL);
	glCompileShader(fshader);
	check_status(fshader, false);

	// Shader program.
	program_id = glCreateProgram();
	glAttachShader(program_id, vshader);
	glAttachShader(program_id, fshader);
	glLinkProgram(program_id);
	check_status(program_id, true);

	// Delete shaders we no longer need anymore.
	glDeleteShader(vshader);
	glDeleteShader(fshader);
}

void Shader::use()
{
	// TODO: return a reference to the used shader program id?
	glUseProgram(program_id);
}

void Shader::set_uniform_float(const GLchar* name, GLfloat value)
{
	GLint loc = glGetUniformLocation(program_id, name);
	glUniform1f(loc, value);
}

void Shader::set_uniform_integer(const GLchar* name, GLint value)
{
	GLint loc = glGetUniformLocation(program_id, name);
	glUniform1i(loc, value);
}

void Shader::set_uniform_vector2f(const GLchar* name, GLfloat x, GLfloat y)
{
	GLint loc = glGetUniformLocation(program_id, name);
	glUniform2f(loc, x, y);
}

void Shader::set_uniform_vector3f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z)
{
	GLint loc = glGetUniformLocation(program_id, name);
	glUniform3f(loc, x, y, z);
}

void Shader::set_uniform_vector4f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	GLint loc = glGetUniformLocation(program_id, name);
	glUniform4f(loc, x, y, z, w);
}

void Shader::set_uniform_vector4f(const GLchar* name, const glm::vec4& v4)
{
	GLint loc = glGetUniformLocation(program_id, name);
	glUniform4f(loc, v4.x, v4.y, v4.z, v4.w);
}

void Shader::set_uniform_matrix4f(const GLchar* name, const glm::mat4& matrix)
{
	GLint loc = glGetUniformLocation(program_id, name);
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::check_status(GLuint id, bool is_program)
{
	int success;
	char infoLog[512];

	if (is_program)
	{
		glGetProgramiv(id, GL_LINK_STATUS, &success);

		if (!success)
		{
			glGetProgramInfoLog(id, sizeof(infoLog), NULL, infoLog);

			std::cout << "Error: shader program fail, " << infoLog << std::endl;
		}
	}
	else
	{
		glGetShaderiv(id, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(id, sizeof(infoLog), NULL, infoLog);
			std::cout << "Error: shader fail, " << infoLog << std::endl;

		}
	}
}