// STRIPPED SOUND SYSTEM FOR M5StickC Plus
// All functions are stubs to save memory

#include "doomstat.h"
#include "s_sound.h"
#include "sounds.h"
#include "global_data.h"
#include "p_mobj.h"

void S_Init(int sfxVolume, int musicVolume) {}
void S_Stop(void) {}
void S_Start(void) {}
void S_StartSoundAtVolume(mobj_t *origin, int sfx_id, int volume) {}
void S_StartSound(mobj_t *origin, int sfx_id) {}
void S_StartSound2(degenmobj_t* origin, int sfx_id) {}
void S_StopSound(void* origin) {}
void S_StopChannel(int cnum) {}
void S_UpdateSounds(void* listener) {}
void S_SetMusicVolume(int volume) {}
void S_SetSfxVolume(int volume) {}
void S_StartMusic(int m_id) {}
void S_ChangeMusic(int musicnum, int looping) {}
void S_StopMusic(void) {}
void S_PauseSound(void) {}
void S_ResumeSound(void) {}
int S_AdjustSoundParams(mobj_t *listener, mobj_t *source, int *vol, int *sep) { return 0; }
