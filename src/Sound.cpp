#include "pch.h"
#include "loader.h"
#include "Sound.h"
#include "winmain.h"

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

#include <kos.h>
#include <dc/sound/aica_comm.h>
#include <dc/sound/sound.h>
#include <dc/sound/sfxmgr.h>

int Sound::num_channels;
bool Sound::enabled_flag = false;
int* Sound::TimeStamps = nullptr;

/*
 *    temporary band-aid fix for loading 8-bit mono PCM samples
 *    they appear to not be loaded correctly through KOS' snd_sfx_load() function
 *    most of this code is from snd_sfxmgr.c with some modifications
 *    (https://github.com/KallistiOS/KallistiOS/blob/master/kernel/arch/dreamcast/sound/snd_sfxmgr.c)
 */

struct snd_effect;
LIST_HEAD(selist, snd_effect);

extern struct selist snd_effects;
	
typedef struct snd_effect {
    uint32_t  locl, locr;
    uint32_t  len;
    uint32_t  rate;
    uint32_t  used;
    uint32_t  fmt;
    uint16_t  stereo;

    LIST_ENTRY(snd_effect)  list;
} snd_effect_t;

static snd_effect_t *create_snd_effect(drwav *wavhdr, uint8_t *wav_data) {
    snd_effect_t *effect;
    uint32_t len, rate;
    uint16_t channels;
    
    effect = (snd_effect_t*)malloc(sizeof(snd_effect_t));
    if(effect == NULL)
        return NULL;

    memset(effect, 0, sizeof(snd_effect_t));

    channels = wavhdr->fmt.channels;
    rate = wavhdr->fmt.sampleRate;
    len = wavhdr->dataChunkDataSize*2;

    effect->rate = rate;
    effect->stereo = channels > 1;

	// just hardcode this for now. the game's sounds are 8-bit mono samples,
	// which are then converted to 16-bit mono by dr_wav
	effect->fmt = AICA_SM_16BIT;
	effect->len = len / 2;

	effect->locl = snd_mem_malloc(len);
	if(effect->locl)
		spu_memload_sq(effect->locl, wav_data, len);

	effect->locr = 0;
	return effect;
}

static sfxhnd_t snd_sfx_load_alt(const char *fn) {
    drwav wavfp;
    snd_effect_t *effect;

    if (!drwav_init_file(&wavfp, fn, NULL))
		return SFXHND_INVALID;

    int16_t* pSampleData = new int16_t[wavfp.totalPCMFrameCount * wavfp.channels];
	if (pSampleData == 0)
	{
		drwav_uninit(&wavfp);
		return SFXHND_INVALID;
	}

	uint32_t totalRead = drwav_read_pcm_frames(&wavfp, wavfp.totalPCMFrameCount, pSampleData);
	if (!totalRead)
	{
		drwav_uninit(&wavfp);
		delete[] pSampleData;
		return SFXHND_INVALID;
	}

	if (wavfp.bitsPerSample == 8) // 8 bit
	{
		int16_t* _8bitdata = new int16_t[wavfp.totalPCMFrameCount * wavfp.channels];
		drwav_u8_to_s16((drwav_int16*)_8bitdata, (drwav_uint8*)pSampleData, wavfp.totalPCMFrameCount);
		delete[] pSampleData;
		pSampleData = _8bitdata;
	}

    /* Create and initialize sound effect */
    effect = create_snd_effect(&wavfp, (uint8_t*)pSampleData);
    if(!effect) {
		drwav_uninit(&wavfp);
        delete[] pSampleData;
        return SFXHND_INVALID;
    }

    /* Finish up and return the sound effect handle */
	delete[] pSampleData;
	drwav_uninit(&wavfp);

    LIST_INSERT_HEAD(&snd_effects, effect, list);

    return (sfxhnd_t)effect;
}


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
	if (!enabled_flag || !buf) return;
	int channel = snd_sfx_play((sfxhnd_t)buf, 255, 128);
	if (channel >= 0)
		TimeStamps[channel] = time;
}

uint8_t* Sound::LoadWaveFile(const std::string& lpName)
{
	sfxhnd_t snd = snd_sfx_load_alt(lpName.c_str());
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
