#pragma once

#include <openal/al.h>
#include <openal/alc.h>

struct Audio_Clip
{
	ALuint id;
};

struct Audio_Source
{
	ALuint id;
};

class Audio
{
public:
	
	 Audio();
	~Audio();

	Audio_Clip load_wav_from_file(const char* file_path);
	Audio_Clip load_ogg_from_file(const char* file_path);

	Audio_Source create_audio_source(float volume = 1.0f, float pitch = 1.0f);

	bool is_source_playing(Audio_Source source);

	void play(Audio_Source source, Audio_Clip clip, bool looping = false);
	void pause(Audio_Source source);
	void unpause(Audio_Source source);
	void stop(Audio_Source source);

private:

	ALCdevice*  al_device;
	ALCcontext* al_context;
	
	void init();
};