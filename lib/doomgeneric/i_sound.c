// Dummy sound implementation for M5StickC Plus (no audio hardware)
#include "doomtype.h"
#include "i_sound.h"
#include "sounds.h"
#include "m_config.h"

// Sound device variable (required by config system)
int snd_musicdevice = 0;

// Dummy implementations - M5StickC Plus has no audio hardware
void I_InitSound(boolean use_sfx_prefix) {}
void I_ShutdownSound(void) {}
int I_GetSfxLumpNum(sfxinfo_t* sfx) { return 0; }
void I_UpdateSound(void) {}
void I_UpdateSoundParams(int channel, int vol, int sep) {}
int I_StartSound(sfxinfo_t *sfxinfo, int channel, int vol, int sep) { return 0; }
void I_StopSound(int channel) {}
boolean I_SoundIsPlaying(int channel) { return false; }
void I_PrecacheSounds(sfxinfo_t *sounds, int num_sounds) {}

// Music functions (all dummies)
void I_InitMusic(void) {}
void I_ShutdownMusic(void) {}
void I_SetMusicVolume(int volume) {}
void I_PauseSong(void) {}
void I_ResumeSong(void) {}
void *I_RegisterSong(void *data, int len) { return NULL; }
void I_UnRegisterSong(void *handle) {}
void I_PlaySong(void *handle, boolean looping) {}
void I_StopSong(void) {}
boolean I_MusicIsPlaying(void) { return false; }

// Bind sound variables to config system
void I_BindSoundVariables(void) {
    M_BindVariable("snd_musicdevice", &snd_musicdevice);
}
