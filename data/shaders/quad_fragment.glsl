#version 330 core

in vec2 tex_coords;

out vec4 outcolor;

uniform sampler2D tex;
uniform vec2 tiling;
uniform vec4 color;

void main()
{
	outcolor = texture(tex, tex_coords * tiling) * color;
}