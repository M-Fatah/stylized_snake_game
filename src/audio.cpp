#include "audio.h"

#include <iostream>


#define DR_WAV_IMPLEMENTATION
#include "ext/dr_wav.h"

#include "ext/stb_vorbis.h"

Audio::Audio()
{
	init();
}

Audio::~Audio()
{
	// Destroy the context.
	alcMakeContextCurrent(NULL);
	alcDestroyContext(this->al_context);
	
	// Close the device.
	alcCloseDevice(this->al_device);
}

// TODO: Implement stereo support for wav format.
// TODO: Figure out why stereo wav files play on one ear (right side) only.
// Note: Currently supports only mono for wav format.
//       Stereo works fine for ogg format.
void Audio::init()
{
	al_device = alcOpenDevice(alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER));

	if (!al_device)
	{
		std::cerr << "ERROR [OpenAL]: audio device opening failed! (" << alGetError() << ")" << std::endl;

		return;
	}

	//const ALint attribute_list[] = { ALC_FREQUENCY, 44100, 0 };

	//al_context = alcCreateContext(al_device, attribute_list);

	al_context = alcCreateContext(al_device, NULL);

	if (alcMakeContextCurrent(al_context))
	{
		// Set listener position.
		alListener3f(AL_POSITION, 0, 0, 1);
		alListener3f(AL_VELOCITY, 0, 0, 0);
	}
	else
	{
		std::cerr << "ERROR [OpenAL]: audio context failed! (" << alGetError() << ")" << std::endl;

		return;
	}
}

Audio_Source Audio::create_audio_source(float volume, float pitch)
{
	Audio_Source source;

	alGenSources(1, &source.id);
	alSourcef(source.id, AL_GAIN, volume);
	alSourcef(source.id, AL_PITCH, pitch);
	alSource3f(source.id, AL_POSITION, 0, 0, 0);

	return source;
}


void Audio::play(Audio_Source source, Audio_Clip clip, bool looping)
{
	if (is_source_playing(source))
	{
		stop(source);
	}

	alSourcei(source.id, AL_BUFFER, clip.id);
	alSourcei(source.id, AL_LOOPING, looping);
	alSourcePlay(source.id);
}

void Audio::pause(Audio_Source source)
{
	alSourcePause(source.id);
}

void Audio::unpause(Audio_Source source)
{
	alSourcePlay(source.id);
}

void Audio::stop(Audio_Source source)
{
	alSourceStop(source.id);
}

// TODO: Pause playing sources when the game window is minimized then continue playing when it gets restored.
bool Audio::is_source_playing(Audio_Source source)
{
	int playing = 0;
	alGetSourcei(source.id, AL_SOURCE_STATE, &playing);
	return playing == AL_PLAYING;
}

// TODO: Collapse loading functions into one.
Audio_Clip Audio::load_wav_from_file(const char* file_path)
{
	drwav wav;
	if (!drwav_init_file(&wav, file_path)) 
	{
		std::cerr << "ERROR! opening file " << file_path << " failed!" << std::endl;
	}

	int32_t* data = (int32_t*)malloc(wav.totalSampleCount * wav.channels * sizeof(int32_t));
	drwav_read_s32(&wav, wav.totalSampleCount, (dr_int32*)data);

	ALenum format = 0;
	if (wav.bitsPerSample == 16) 
	{
		if (wav.channels == 1) 
		{
			format = AL_FORMAT_MONO16;
		}
		else if (wav.channels == 2) 
		{
			format = AL_FORMAT_STEREO16;
		}
	}
	else if (wav.bitsPerSample == 8) 
	{
		if (wav.channels == 1) 
		{
			format = AL_FORMAT_MONO8;
		}
		else if (wav.channels == 2) 
		{
			format = AL_FORMAT_STEREO8;
		}
	}

	Audio_Clip clip;
	alGenBuffers(1, &clip.id);
	alBufferData(clip.id, format, data, wav.totalSampleCount * wav.bytesPerSample * wav.channels, wav.sampleRate);

	drwav_free(data);

	return clip;
}

// TODO: Proper error handling in case the file is not found or couldn't be loaded.
// TODO: Optimize! loading an ogg files takes way too long at startup.
Audio_Clip Audio::load_ogg_from_file(const char* file_path)
{
	Audio_Clip clip = { 0 };

	ALenum format = 0;

	//int error;
	//stb_vorbis* vorbis = stb_vorbis_open_filename(file_path, &error, NULL);

	//stb_vorbis_info info = stb_vorbis_get_info(vorbis);
	//
	//if (info.max_frame_size > 0)
	//{
	//	if (info.channels == 1) 
	//	{
	//		format = AL_FORMAT_MONO16;
	//	}
	//	else if (info.channels == 2) 
	//	{
	//		format = AL_FORMAT_STEREO16;
	//	}

	//	alGenBuffers(1, &clip.id);
	//	alBufferData(clip.id, format, (char*)info.setup_memory_required, info.max_frame_size, info.sample_rate);

	//	stb_vorbis_close(vorbis);
	//}

	short* data;
	int channels;
	int length;
	int sample_rate;

	length = stb_vorbis_decode_filename(file_path, &channels, &sample_rate, &data);

	if (length > 0) 
	{
		if (channels == 1) 
		{
			format = AL_FORMAT_MONO16;
		}
		else if (channels == 2) 
		{
			format = AL_FORMAT_STEREO16;
		}

		alGenBuffers(1, &clip.id);
		alBufferData(clip.id, format, data, length * channels * sizeof(short), sample_rate);
		
		free(data);
	}

	return clip;
}
