#pragma once

#include <map>

#include "shader.h"
#include "texture.h"
#include "audio.h"

class Assets
{
public:

	static std::map<const char*, Shader> shaders;
	static std::map<const char*, Texture2D> textures;
	static std::map<const char*, Audio_Clip> audio_clips;
	static std::map<const char*, Audio_Source> audio_sources;

	static Shader get_shader(const char* name);

	static Texture2D get_texture(const char* name);

	static Audio_Clip get_audio_clip(const char* name);

	static Audio_Source get_audio_source(const char* name);

	static void dispose();

private:
	
	Assets() { }
};