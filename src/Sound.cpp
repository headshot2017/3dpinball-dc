#include "pch.h"
#include "loader.h"
#include "Sound.h"
#include "winmain.h"

#include <dc/sound/sound.h>
#include <dc/sound/sfxmgr.h>

int Sound::num_channels;
bool Sound::enabled_flag = false;
int* Sound::TimeStamps = nullptr;

bool Sound::Init(int channels, bool enableFlag)
{
	snd_init();

	SetChannels(channels);
	Enable(enableFlag);
	return true;
}

void Sound::Enable(bool enableFlag)
{
	enabled_flag = enableFlag;
}

void Sound::Activate()
{
	
}

void Sound::Deactivate()
{
	
}

void Sound::Close()
{
	snd_sfx_unload_all();
	snd_shutdown();

	delete[] TimeStamps;
	TimeStamps = nullptr;
}

void Sound::PlaySound(uint8_t* buf, int time, int size, int samplerate)
{
	if (!enabled_flag) return;
	int channel = snd_sfx_play((sfxhnd_t)buf, 255, 128);
	if (channel >= 0)
		TimeStamps[channel] = time;
}

uint8_t* Sound::LoadWaveFile(const std::string& lpName)
{
	sfxhnd_t snd = snd_sfx_load(lpName.c_str());
	if (!snd)
		printf("Failed to load sound '%s'\n", lpName.c_str());
	return (uint8_t*)snd;
}

void Sound::FreeSound(uint8_t* wave)
{
	if (wave)
		snd_sfx_unload((sfxhnd_t)wave);
}

void Sound::SetChannels(int channels)
{
	if (channels <= 0)
		channels = 8;

	num_channels = channels;
	delete[] TimeStamps;
	TimeStamps = new int[num_channels]();
}
