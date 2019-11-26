#version 330 core

layout (location = 0) in vec4 vertex;

out vec2 tex_coords;

uniform mat4 projection;
uniform mat4 model;

void main()
{
	gl_Position = projection * model * vec4(vertex.xy, 0.0f, 1.0f);
	tex_coords = vertex.zw;
}