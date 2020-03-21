#version 330 core

layout (location = 0) in vec2 in_vertex;
layout (location = 1) in vec2 in_tex_coords;

out vec2 tex_coords;

uniform mat4 projection;
uniform mat4 model;

void main()
{
	gl_Position = projection * model * vec4(in_vertex, 0.0f, 1.0f);
	tex_coords = in_tex_coords;
}