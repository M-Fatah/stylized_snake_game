#include "assets.h"

std::map<const char*, Shader>       Assets::shaders;
std::map<const char*, Texture2D>    Assets::textures;
std::map<const char*, Audio_Clip>   Assets::audio_clips;
std::map<const char*, Audio_Source> Assets::audio_sources;


Shader Assets::get_shader(const char* name)
{
	return shaders[name];
}

Texture2D Assets::get_texture(const char* name)
{
	return textures[name];
}

Audio_Clip Assets::get_audio_clip(const char* name)
{
	return audio_clips[name];
}

Audio_Source Assets::get_audio_source(const char* name)
{
	return audio_sources[name];
}

void Assets::dispose()
{
	for (auto s : shaders)
		glDeleteProgram(s.second.program_id);

	for (auto t : textures)
		glDeleteTextures(1, &t.second.id);

	for (auto c : audio_clips)
		alDeleteBuffers(1, &c.second.id);

	for (auto s : audio_sources)
		alDeleteSources(1, &s.second.id);
}